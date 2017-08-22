package v;

import static data.Defines.RANGECHECK;

import java.awt.image.BufferedImage;
import java.awt.image.DataBufferInt;
import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;
import java.util.concurrent.Executor;
import java.util.concurrent.Executors;

import rr.column_t;
import rr.patch_t;
import m.BBox;

/** N.B. this renderer just uses 8-bit resources and 256 color, but renders directly onto a 
 *  24-bit canvas, so technically it should be a byte[],byte[] renderer. It's not a "true color" 
 *  renderer in the sense of using extended colormaps etc. 
 * 
 * @author velktron
 *
 */

public class ParallelTrueColorRenderer extends SoftwareVideoRenderer<byte[],byte[]> {
	
static final String rcsid = "$Id: ParallelTrueColorRenderer.java,v 1.3 2016/06/06 14:27:55 velktron Exp $";


/* With a truecolour raster, some things are indeed easier */
protected int[][] palettes;
protected int[] raster;
protected final int nrOfProcessors;
protected Runnable[] paletteThreads;
protected final Executor executor;

public ParallelTrueColorRenderer(){
super();
Runtime runtime = Runtime.getRuntime();
nrOfProcessors = runtime.availableProcessors();
updateBarrier=new CyclicBarrier(nrOfProcessors+1);
paletteThreads=new PaletteThread[nrOfProcessors];
int len=this.getHeight()*this.getWidth();
int chunk=len/nrOfProcessors;
for (int i=0;i<nrOfProcessors;i++){
	paletteThreads[i]=new PaletteThread(i*chunk,(i+1)*chunk);
}
this.executor=Executors.newFixedThreadPool(nrOfProcessors);
}

public ParallelTrueColorRenderer(int w,int h){
    // Defaults
    super(w,h);
    Runtime runtime = Runtime.getRuntime();
    nrOfProcessors = runtime.availableProcessors();
    updateBarrier=new CyclicBarrier(nrOfProcessors+1);
    paletteThreads=new PaletteThread[nrOfProcessors];
    int len=w*h;
    int chunk=len/nrOfProcessors;
    for (int i=0;i<nrOfProcessors;i++){
    	paletteThreads[i]=new PaletteThread(i*chunk,(i+1)*chunk);
    }
    this.executor=Executors.newFixedThreadPool(nrOfProcessors);
}

public void Init () 
{ 
 int		i;
 for (i=0 ; i<screens.length ; i++){
	screens[i] = new byte[this.getHeight()*this.getWidth()];
	}
     dirtybox=new BBox();
     
  // Using ARGB is half the speed, WTF? While RGB is almost as fast as indexed. Go figure.
  this.currentscreen=new BufferedImage(width,height, BufferedImage.TYPE_INT_RGB);
  this.mapInternalRasterToBufferedImage((BufferedImage) currentscreen);
}

@Override
public void setUsegamma(int gammalevel) {
	this.usegamma=gammalevel%maxgammas;
	
}

@Override
public void setPalette(int palette) {
	this.usepalette=palette%maxpalettes;
}

@Override
protected final void specificPaletteCreation(byte[] paldata,
		short[][] gammadata, 
		final int palettes, 
		final int colors,
		final int stride,
		final int gammalevels){

	  System.out.printf("Enough data for %d palettes",maxpalettes);
	  System.out.printf("Enough data for %d gamma levels",maxgammas);
	  
	  this.palettes=new int[maxgammas*maxpalettes][];
	  
	  for (int z=0;z<maxgammas;z++){
		  
		  // For each palette
		  for (int y=0;y<maxpalettes;y++){
			  this.palettes[z*maxpalettes+y]=new int[colors];
			  
			  for (int x=0;x<colors;x++){
				  int r=gammadata[z][0xFF&paldata[y*colors*stride+stride*x]]; // R
				  int g=gammadata[z][0xFF&paldata[1+y*colors*stride+stride*x]]; // G
				  int b=gammadata[z][0xFF&paldata[2+y*colors*stride+stride*x]]; // B
				  int color=0xFF000000|r<<16|g<<8|b;
				  this.palettes[z*maxpalettes+y][x]=color;
			  	}
	  		}
	  }

}

/** Hotlinks a 32-bit "canvas" (the raster int[] array) to an externally supplied
 *  buffered image. Now whatever we write into raster, will appear in the image as well,
 *  without using drawing primitives. Necessary for fast rendering.
 *  
 * @param b
 */

private void mapInternalRasterToBufferedImage(BufferedImage b){
    raster=((DataBufferInt)(b.getRaster().getDataBuffer())).getData();
    
}

/** Update "canvas" to one of the internal screens.
 *  
 * @param screen
 * @param b
 */

@Override
public  final void update()  {
	
	for (int i=0;i<this.nrOfProcessors;i++){
		executor.execute(paletteThreads[i]);
	}
	try {
		updateBarrier.await();
	} catch (InterruptedException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
	} catch (BrokenBarrierException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
	}
	
	/*
    final byte[] scr=this.screens[usescreen];
    final int length=scr.length; 
    final int[] pal=this.palettes[usegamma*maxpalettes+usepalette];
    for (int i=0;i<length;i+=16){
        raster[i]=pal[0xFF&scr[i]];
        raster[i+1]=pal[0xFF&scr[i+1]];
        raster[i+2]=pal[0xFF&scr[i+2]];
        raster[i+3]=pal[0xFF&scr[i+3]];
        raster[i+4]=pal[0xFF&scr[i+4]];
        raster[i+5]=pal[0xFF&scr[i+5]];
        raster[i+6]=pal[0xFF&scr[i+6]];
        raster[i+7]=pal[0xFF&scr[i+7]];
        raster[i+8]=pal[0xFF&scr[i+8]];
        raster[i+9]=pal[0xFF&scr[i+9]];
        raster[i+10]=pal[0xFF&scr[i+10]];
        raster[i+11]=pal[0xFF&scr[i+11]];
        raster[i+12]=pal[0xFF&scr[i+12]];
        raster[i+13]=pal[0xFF&scr[i+13]];
        raster[i+14]=pal[0xFF&scr[i+14]];
        raster[i+15]=pal[0xFF&scr[i+15]];

    } */
}

protected final CyclicBarrier updateBarrier;

private class PaletteThread implements Runnable{

	private final int start;
	private final int stop;
	
	public PaletteThread(int start, int stop){
		this.start=start;
		this.stop=stop;
	}
	
	@Override
	public void run() {
		
    	final byte[] scr=screens[usescreen];
    	final int[] pal=palettes[usegamma*maxpalettes+usepalette];
	    for (int i=start;i<stop;i+=16){
	    	raster[i]=pal[0xFF&scr[i]];
	        raster[i+1]=pal[0xFF&scr[i+1]];
	        raster[i+2]=pal[0xFF&scr[i+2]];
	        raster[i+3]=pal[0xFF&scr[i+3]];
	        raster[i+4]=pal[0xFF&scr[i+4]];
	        raster[i+5]=pal[0xFF&scr[i+5]];
	        raster[i+6]=pal[0xFF&scr[i+6]];
	        raster[i+7]=pal[0xFF&scr[i+7]];
	        raster[i+8]=pal[0xFF&scr[i+8]];
	        raster[i+9]=pal[0xFF&scr[i+9]];
	        raster[i+10]=pal[0xFF&scr[i+10]];
	        raster[i+11]=pal[0xFF&scr[i+11]];
	        raster[i+12]=pal[0xFF&scr[i+12]];
	        raster[i+13]=pal[0xFF&scr[i+13]];
	        raster[i+14]=pal[0xFF&scr[i+14]];
	        raster[i+15]=pal[0xFF&scr[i+15]];

	    }
        try {
			updateBarrier.await();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (BrokenBarrierException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
}

public final void DrawPatch
( int       x,
int     y,
int     scrn,
patch_t patch ) 
{ 

 column_t   column; 
 int    desttop;
 final  byte[] dest=screens[scrn];
 int        w; 
     
 y -= patch.topoffset; 
 x -= patch.leftoffset; 
if (RANGECHECK) 
 if (doRangeCheck(x,y,patch,scrn))
 {
   System.err.print("Patch at "+x+","+y+" exceeds LFB\n");
   // No I_Error abort - what is up with TNT.WAD?
   System.err.print("V_DrawPatch: bad patch (ignored)\n");
   return;
 }

 if (scrn==0)
    this.MarkRect (x, y, patch.width, patch.height); 

     
 w = patch.width; 
 desttop = x+this.width*y; 
 // For each column..
 int destPos;
 int ptr=0;
 for (int col=0 ; col<w ; desttop++, col++,x++)
 { 
    // This points at a "column" object.     
    //column = (column_t *)((byte *)patch + LONG(patch->columnofs[col])); 
    column=patch.columns[col];
    // For each post...
    for (int i=0;i<column.posts;i++){
        // Get pointer to post offset.
        ptr=column.postofs[i];
        // Get post delta
        short delta=column.postdeltas[i];
        // We skip delta, len and padding.
        // ptr+=3; NOT NEEDED< pre-skipped at parsing. 
        
        // Skip transparent rows...
        if (delta==0xFF) break;

        destPos = desttop + delta*this.width;  
        
        // These lengths are already correct.
        for (int j=0;j<column.postlen[i];j++){
               dest[destPos] = column.data[ptr++];
               destPos += this.width;
        }
    }
 }
    
    
}

}

