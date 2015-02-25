#!/usr/bin/env python

import numpy as np
import matplotlib
matplotlib.use('Tkagg')
import matplotlib.pyplot as plt
import sys
import os
from itertools import izip, imap
from copy import deepcopy
import Tkinter
import tkFileDialog
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

"""
This is a python script which can be used to easily edit TIRIFIC def files.

The usage is simple: just give a tirific def file name, select a parameter,
fiddle with the values using the mouse to drag them, and then write out a
new def file. You can modify multiple parameters: just write the file each
time you finish working on one! (Previous modifications are retained.)

It uses class DraggableRectangle from:
<http://matplotlib.sourceforge.net/users/event_handling.html>
as well as odict (ordered dictionary) from:
<http://dev.pocoo.org/hg/sandbox/raw-file/tip/odict.py>

v1.0:  written 22 March 2011 by George Heald
v1.1:  10/04/2011 by GHH
        * add browse buttons for input and output files
        * allow more than one def file to be loaded
v1.11: 13/04/2011 by GHH
	* small bugfix (converting NUR to int assumed no decimal point)

To do:
- implement scaling, median filtering
- undo?
- allow editing OUTSET and have a button running tirific_new in nhermes?
"""

version = 'v1.11 GHH 13/04/2011'

#The following list gives the parameters that can be edited with this
#script. The script checks for the first part of the name and also for
#the same plus '_', so that multiple disks as specified by tirific
#are supported. (e.g. both VROT and VROT_2 are allowed given this
#list). Any other parameters needed should be added here.
editableParams = ['VROT','Z0','SBR','INCL','PA','XPOS','YPOS','VSYS','SDIS',
	'VRAD','VVER','DVRO','DVRA','DVVE','WM0A','WM1A','WM1P',
	'WM2A','WM2P','WM3A','WM3P','WM4A','WM4P','LS0','LC0','SM0A',
	'SM1A','SM1P','SM2A','SM2P','SM3A','SM3P','SM4A','SM4P','VM0A',
	'VM1A','VM1P','VM2A','VM2P','VM3A','VM3P','VM4A','VM4P',
	'GA1A','GA1P','GA2A','GA2P','GA3A','GA3P','GA4A','GA4P',
	'GA1D','GA2D','GA3D','GA4D','AZ1P','AZ2P','AZ1W','AZ2W','CLNR']


class DraggableRectangle:
    # draggable rectangle with the animation blit techniques; see
    # http://www.scipy.org/Cookbook/Matplotlib/Animations
    # edited to work with Line2D objects instead of the original histograms
    lock = None  # only one can be animated at a time
    def __init__(self, line):
        self.line = line
        self.press = None
        self.background = None

    def connect(self):
        #connect to all the events we need
        self.cidpress = self.line.figure.canvas.mpl_connect(
            'button_press_event', self.on_press)
        self.cidrelease = self.line.figure.canvas.mpl_connect(
            'button_release_event', self.on_release)
        self.cidmotion = self.line.figure.canvas.mpl_connect(
            'motion_notify_event', self.on_motion)

    def on_press(self, event):
        #on button press we see if the mouse is over us and store some data
        if event.inaxes != self.line.axes: return
        if DraggableRectangle.lock is not None: return
        contains, attrd = self.line.contains(event)
        if not contains: return
        x0 = self.line.get_xdata()
	y0 = self.line.get_ydata()
        self.press = x0, y0, event.xdata, event.ydata
        DraggableRectangle.lock = self

        # draw everything but the selected rectangle and store the pixel buffer
        canvas = self.line.figure.canvas
        axes = self.line.axes
        self.line.set_animated(True)
        canvas.draw()
        self.background = canvas.copy_from_bbox(self.line.axes.bbox)

        # now redraw just the rectangle
        axes.draw_artist(self.line)

        # and blit just the redrawn area
        canvas.blit(axes.bbox)

    def on_motion(self, event):
        #on motion we will move the line if the mouse is over us
        if DraggableRectangle.lock is not self:
            return
        if event.inaxes != self.line.axes: return
        x0, y0, xpress, ypress = self.press
        dx = 0.#event.xdata - xpress
        dy = event.ydata - ypress
        self.line.set_xdata(x0+dx)
        self.line.set_ydata(y0+dy)

        canvas = self.line.figure.canvas
        axes = self.line.axes
        # restore the background region
        canvas.restore_region(self.background)

        # redraw just the current rectangle
        axes.draw_artist(self.line)

        # blit just the redrawn area
        canvas.blit(axes.bbox)

    def on_release(self, event):
        #on release we reset the press data
        if DraggableRectangle.lock is not self:
            return

        self.press = None
        DraggableRectangle.lock = None

        # turn off the rect animation property and reset the background
        self.line.set_animated(False)
        self.background = None

        # redraw the full figure
        self.line.figure.canvas.draw()

    def disconnect(self):
        #disconnect all the stored connection ids
        self.line.figure.canvas.mpl_disconnect(self.cidpress)
        self.line.figure.canvas.mpl_disconnect(self.cidrelease)
        self.line.figure.canvas.mpl_disconnect(self.cidmotion)

missing = object()

class odict(dict):
    """
    Ordered dict example implementation.

    This is the proposed interface for a an ordered dict as proposed on the
    Python mailinglist (proposal_).

    It's a dict subclass and provides some list functions.  The implementation
    of this class is inspired by the implementation of Babel but incorporates
    some ideas from the `ordereddict`_ and Django's ordered dict.

    The constructor and `update()` both accept iterables of tuples as well as
    mappings:

    >>> d = odict([('a', 'b'), ('c', 'd')])
    >>> d.update({'foo': 'bar'})
    >>> d
    odict.odict([('a', 'b'), ('c', 'd'), ('foo', 'bar')])
    
    Keep in mind that when updating from dict-literals the order is not
    preserved as these dicts are unsorted!

    You can copy an odict like a dict by using the constructor, `copy.copy`
    or the `copy` method and make deep copies with `copy.deepcopy`:

    >>> from copy import copy, deepcopy
    >>> copy(d)
    odict.odict([('a', 'b'), ('c', 'd'), ('foo', 'bar')])
    >>> d.copy()
    odict.odict([('a', 'b'), ('c', 'd'), ('foo', 'bar')])
    >>> odict(d)
    odict.odict([('a', 'b'), ('c', 'd'), ('foo', 'bar')])
    >>> d['spam'] = []
    >>> d2 = deepcopy(d)
    >>> d2['spam'].append('eggs')
    >>> d
    odict.odict([('a', 'b'), ('c', 'd'), ('foo', 'bar'), ('spam', [])])
    >>> d2
    odict.odict([('a', 'b'), ('c', 'd'), ('foo', 'bar'), ('spam', ['eggs'])])

    All iteration methods as well as `keys`, `values` and `items` return
    the values ordered by the the time the key-value pair is inserted:

    >>> d.keys()
    ['a', 'c', 'foo', 'spam']
    >>> d.values()
    ['b', 'd', 'bar', []]
    >>> d.items()
    [('a', 'b'), ('c', 'd'), ('foo', 'bar'), ('spam', [])]
    >>> list(d.iterkeys())
    ['a', 'c', 'foo', 'spam']
    >>> list(d.itervalues())
    ['b', 'd', 'bar', []]
    >>> list(d.iteritems())
    [('a', 'b'), ('c', 'd'), ('foo', 'bar'), ('spam', [])]

    Index based lookup is supported too by `byindex` which returns the
    key/value pair for an index:

    >>> d.byindex(2)
    ('foo', 'bar')

    You can reverse the odict as well:

    >>> d.reverse()
    >>> d
    odict.odict([('spam', []), ('foo', 'bar'), ('c', 'd'), ('a', 'b')])
    
    And sort it like a list:

    >>> d.sort(key=lambda x: x[0].lower())
    >>> d
    odict.odict([('a', 'b'), ('c', 'd'), ('foo', 'bar'), ('spam', [])])

    .. _proposal: http://thread.gmane.org/gmane.comp.python.devel/95316
    .. _ordereddict: http://www.xs4all.nl/~anthon/Python/ordereddict/
    """

    def __init__(self, *args, **kwargs):
        dict.__init__(self)
        self._keys = []
        self.update(*args, **kwargs)

    def __delitem__(self, key):
        dict.__delitem__(self, key)
        self._keys.remove(key)

    def __setitem__(self, key, item):
        if key not in self:
            self._keys.append(key)
        dict.__setitem__(self, key, item)

    def __deepcopy__(self, memo=None):
        if memo is None:
            memo = {}
        d = memo.get(id(self), missing)
        if d is not missing:
            return d
        memo[id(self)] = d = self.__class__()
        dict.__init__(d, deepcopy(self.items(), memo))
        d._keys = self._keys[:]
        return d

    def __getstate__(self):
        return {'items': dict(self), 'keys': self._keys}

    def __setstate__(self, d):
        self._keys = d['keys']
        dict.update(d['items'])

    def __reversed__(self):
        return reversed(self._keys)

    def __eq__(self, other):
        if isinstance(other, odict):
            if not dict.__eq__(self, other):
                return False
            return self.items() == other.items()
        return dict.__eq__(self, other)

    def __ne__(self, other):
        return not self.__eq__(other)

    def __cmp__(self, other):
        if isinstance(other, odict):
            return cmp(self.items(), other.items())
        elif isinstance(other, dict):
            return dict.__cmp__(self, other)
        return NotImplemented

    @classmethod
    def fromkeys(cls, iterable, default=None):
        return cls((key, default) for key in iterable)

    def clear(self):
        del self._keys[:]
        dict.clear(self)

    def copy(self):
        return self.__class__(self)

    def items(self):
        return zip(self._keys, self.values())

    def iteritems(self):
        return izip(self._keys, self.itervalues())

    def keys(self):
        return self._keys[:]

    def iterkeys(self):
        return iter(self._keys)

    def pop(self, key, default=missing):
        if default is missing:
            return dict.pop(self, key)
        elif key not in self:
            return default
        self._keys.remove(key)
        return dict.pop(self, key, default)

    def popitem(self, key):
        self._keys.remove(key)
        return dict.popitem(key)

    def setdefault(self, key, default=None):
        if key not in self:
            self._keys.append(key)
        dict.setdefault(self, key, default)

    def update(self, *args, **kwargs):
        sources = []
        if len(args) == 1:
            if hasattr(args[0], 'iteritems'):
                sources.append(args[0].iteritems())
            else:
                sources.append(iter(args[0]))
        elif args:
            raise TypeError('expected at most one positional argument')
        if kwargs:
            sources.append(kwargs.iteritems())
        for iterable in sources:
            for key, val in iterable:
                self[key] = val

    def values(self):
        return map(self.get, self._keys)

    def itervalues(self):
        return imap(self.get, self._keys)

    def index(self, item):
        return self._keys.index(item)

    def byindex(self, item):
        key = self._keys[item]
        return (key, dict.__getitem__(self, key))

    def reverse(self):
        self._keys.reverse()

    def sort(self, *args, **kwargs):
        self._keys.sort(*args, **kwargs)

    def __repr__(self):
        return 'odict.odict(%r)' % self.items()

    __copy__ = copy
    __iter__ = iterkeys

class TirEditor(Tkinter.Tk):

	def __init__(self, parent):
		Tkinter.Tk.__init__(self,parent)
		self.parent = parent
		self.protocol("WM_DELETE_WINDOW",self.dest)
		self.hasPlotBtn = False
		self.firstRead = True 
		self.main()

	def doPlot(self):

		keytoplot = self.keytoplot.get()
		# we should have keys NUR and RADI, if not there's a problem!
		nrings = int(float(self.tirkeys['NUR']))
		myxdata = self.tirkeys['RADI'].split()
		myydata = self.tirkeys[keytoplot].split()
		for i in range(nrings):
			myxdata[i]=float(myxdata[i])
			try:
				myydata[i]=float(myydata[i])
			except:
				myydata[i]=myydata[i-1]
		self.ax.clear()
		connect_line = self.ax.plot(myxdata,myydata,'k--')
		self.lines = []
		for i in range(nrings):
			self.lines.append(self.ax.plot(myxdata[i],
				myydata[i],'ro')[0])
		self.drs = []
		for line in self.lines:
	    		self.dr = DraggableRectangle(line)
	    		self.dr.connect()
	    		self.drs.append(self.dr)

		plt.xlabel('RADI')
		plt.ylabel(keytoplot)

		self.canvas.draw()
		if not self.hasPlotBtn:
			self.btn3.config(state=Tkinter.NORMAL)
			self.hasPlotBtn = True

	def writeNewTirDef(self):
		newydata=''
		if self.keytoplot.get()[0:4]=='XPOS' or self.keytoplot.get()[0:4]=='YPOS':
			for line in self.lines:
				newydata+='%+9.8E '%line.get_ydata()[0]
		else:
			for line in self.lines:
				newydata+='%+6.5E '%line.get_ydata()[0]

		print 'Before:', self.tirkeys[self.keytoplot.get()]
		print ' After:', newydata[:-1]

		f = open(self.outfilename.get(),'w')
		for k,v in self.tirkeys.iteritems():
			if k == self.keytoplot.get():
				print '(New value for %s used in output)'%(
					self.keytoplot.get())
				print >>f, k+'='+newydata[:-1]
				self.tirkeys[self.keytoplot.get()] = newydata
			elif k[0:5] == 'BLANK':
				print >>f, ' '
			else:
				print >>f, k+'= '+v
		f.close()

		print 'Wrote output def file %s'%(self.outfilename.get())

	def readTirDef(self):
		infile = self.tirfilename.get()
		print 'Reading tirific def file',infile
		f = open(infile)
		numblanks=0
		parlist=[]
		for line in f:
			sline = line.split('=')
			if len(sline) == 2:
				self.tirkeys[sline[0].strip()]=sline[1].strip()
				for p in editableParams:
				    if sline[0].strip() == p:
					parlist.append(sline[0].strip())
					break
				    elif sline[0].strip()[0:len(p)+1] == p+'_':
					parlist.append(sline[0].strip())
					break
			elif len(sline) == 1:
				if sline[0].strip() == '':
					self.tirkeys['BLANK%03d'%numblanks]=''
					numblanks+=1
				else:
					self.tirkeys[sline[0].strip()]=''
			else:
				print 'Error reading def file'
				sys.exit(1)
		f.close()
		#self.btn.config(text='Read succeeded',state=Tkinter.DISABLED)
		print('Read succeeded')
		self.choose = Tkinter.OptionMenu(self, self.keytoplot, *parlist)
		self.keytoplot.set(parlist[0])
		self.choose.grid(column=0,row=2,columnspan=2)
		self.btn2.config(state=Tkinter.NORMAL)
		if self.firstRead:
			self.firstRead = False
		else:
			print "*** WARNING: Any prior parameter changes have been LOST!"

	def dest(self):
		self.destroy()
		sys.exit()

	def main(self):

		self.tirkeys = odict({})

		self.fig = plt.figure()
		self.ax = self.fig.add_subplot(111)
		self.frame = Tkinter.Frame(self)
		self.frame.grid(columnspan=3,padx=15,pady=15,row=0)
		self.canvas = FigureCanvasTkAgg(self.fig,master=self.frame)
		self.canvas._tkcanvas.config(borderwidth=0,highlightthickness=0)
		self.canvas.get_tk_widget().grid()
		self.canvas._tkcanvas.grid()
		self.tirfilename = Tkinter.StringVar()
		self.txt = Tkinter.Entry(self, 
			textvariable=self.tirfilename,width=50)
		self.tirfilename.set('Enter input def file name here')
		self.txt.grid(column=0,row=1,sticky='E')
		self.browsebtn = Tkinter.Button(self,text='Browse...',
			command=self.gettirlocation)
		self.browsebtn.grid(column=1,row=1,sticky='W')
		self.btn = Tkinter.Button(self,text='Read file',
			command=self.readTirDef)
		self.btn.grid(column=2,row=1)
		self.keytoplot = Tkinter.StringVar()
		self.choose = Tkinter.OptionMenu(self, self.keytoplot, '')
		self.keytoplot.set('')
		self.choose.grid(column=0,row=2,columnspan=2)
		self.btn2 = Tkinter.Button(self,text='Plot values',
			command=self.doPlot,state=Tkinter.DISABLED)
		self.btn2.grid(column=2,row=2)
		self.label = Tkinter.Label(self,
			text='After loading input: edit as many parameters '+
			'as needed, writing an output file each time.',width=75)
		self.label.grid(column=0,row=3,columnspan=3)
		self.outfilename = Tkinter.StringVar()
		self.txt2 = Tkinter.Entry(self, 
			textvariable=self.outfilename,width=50)
		self.outfilename.set('output.def')
		self.txt2.grid(column=0,row=4,sticky='E')
		self.outbrowsebtn = Tkinter.Button(self,text='Browse...',
			command=self.puttirlocation)
		self.outbrowsebtn.grid(column=1,row=4,sticky='W')
		self.btn3 = Tkinter.Button(self,text='Write output .def',
			command=self.writeNewTirDef,state=Tkinter.DISABLED)
		self.btn3.grid(column=2,row=4,pady=15)
		# The following is a small tirific logo, converted to a
		# base64-encoded string using 'base64.encode()'
		self.tirificIcon = """
R0lGODlhNAASAPfvAOUZJ+wZJ+UcKescKNslL9crNeAgLOgnNOMwO+40Pds1QNQ9Ru07Q5N6bY99
edxTWeFJT+5PVepPWPBMVe9QUe5SWO9YWNRbYdxaYdlrcdpxdtx2e/BkY/Jqb+1sc+R1efJ3dRCe
WQSiVwSjWAqiWhKjXRylYxyxbiOjZi6kbCKvbzKobya0djGxdzS3fTO4fkCndkisffiXJvmbKfKS
MfCUOPWbO/qoNvejOem2Ov/nHvnOKP3cJPzfLPTHMPnLMv7XN/7hI//rJ//kKaOLXaGcXLmaX5yF
dp2Lf7OTYrWaYqaSc6uef722ZaGidO+eTfGbRO6fV8ysVMS2WvepRfmoR/WnT/isSPWsXO2mad/M
Qd/CS/3mTfbkV8zHcvnGauPbcPfobSNDmShGmStKnTBNnzVQnTtRmi1NoTBPoTRTojpWpD1apD5f
qk5ck0Nanktgk1BkklZplFBjn19qmHFtgXF9i0JcpU1goURiqlBkpF5up19wpVpwqEprsV51sVl4
uGh0p2R4p2NzqmV4rGt7rWR4tWp8t3B/sdp9ge14gPV4gUy0hUG7h0+/j1O+kFy9kmq2k2q5lXO8
nHq7nXKCp3uHs3aGuHuLvnm+oU/BklTBlGTGnWbHoG3HoXTCoHbMqX7JqXKQxX+Xx5ueh6erhrGz
kJOeqIGMtYORvY2Wu5GfvZiiqoO/o9qKjtyWmeqBg+uIjfKQi9ydoOKgo+aoqeysr+Kwsu68vve8
tfC8vITApInCqZPGr5PIsJfTuqDMuPnKhvXAiP3WgvnMlPnNnfzgnvDIpPPMr+7JsPzasvbevfve
vfzio4ycxpafwYyjzp+owYyl0Z+12ayyzbO3zaOu0K672bG62K3TwqPaxa7dzbDdzrrA2L3k1bXH
4r7O5vDHx/zMzvrVzvbcyvneysfn3Mrm3c3o3/rjyffx2v7328fQ5M7a7P/m5+0cKgAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAEAAO8AIf4VQ3JlYXRl
ZCB3aXRoIFRoZSBHSU1QACwAAAAANAASAAAI/gDfCRxIsKDBds4MKlzIsGFDaWugOZxIcaK1NmMK
VdzIUeA3P2nG6OlIsiC7UX8MHcIEDRCal2Wolez4K9s7bGxe3lnzsmeaVAo/2SSoS2CoXgXVLSs4
LhcsgZ5WCLQ0ps+qQpVMNXFiJ1A0S86wDTzXIobAXLI4PBDICcXAdca+XCkmqwIFCxQSEHj1DlQJ
XwL37CnEKgyQHzx6lBIkaMwaQ9beoXshVRyDAAYusC0B7N26YTdmVFH2gQCA0ww0CAQ1opXAanfU
eBmShM6UIETQqAk5JtC7cy/cikuAoFY4tiR2vWuGI4owZgJdfTggwNXATiMiDbw0pgiPLaXA+wRZ
8nJMGd/vuLEw8c7dBAYgIAjcNILSuy9QFEoQkGigphGSDASNGnBoocMQQvCABBpj3KHKQNqcwN47
HQQgwAK3vNPICJmkQ4UNyBjkAQAbDESffQKJwgYrXfggRRA71IHGHgWp59Y7iwCQgX8j8PIOFjJc
EQwx4wwUi44DcZKcQOD4QQoXPxxhRw85uHGGQd6oINU7ihQwy0COhIDUMU/QYEMW5AxkCwbWCfQI
DANJk4YcRjRwhhxKOFDGHQZpw4hr7+BCC0GQaDdQMslsdE0eY5wRiCrUBDLGS3jMZGkhqFRD0CFj
jDGHpaAahMgYNIZqqkCoPHOqQAEBADs=
"""
		self.canvas1 = Tkinter.Canvas(width=60,height=26,bg='white')
		self.canvas1.grid(column=0,row=5)
		self.gif1 = Tkinter.PhotoImage(data=self.tirificIcon)
		self.canvas1.create_image(30,13,image=self.gif1)
		self.label2 = Tkinter.Label(self,
			text='tiredit.py %s'%(version),width=30)
		self.label2.grid(column=1,row=5,columnspan=2)

	def gettirlocation(self):
		filename = tkFileDialog.askopenfilename(initialdir=os.getcwd(),
			filetypes=[('DEF files','.def'),('All files','.*')],
			title='Choose a TiRiFiC def file to use')
		self.tirfilename.set(filename)

	def puttirlocation(self):
		filename = tkFileDialog.asksaveasfilename(initialdir=os.getcwd(),
			defaultextension='.def',title='Choose an output def file',
			filetypes=[('DEF files','.def'),('All files','.*')])
		self.outfilename.set(filename)


if __name__ == "__main__":
	app = TirEditor(None)
	app.title('TiRiFiC def file editor')
	app.mainloop()

