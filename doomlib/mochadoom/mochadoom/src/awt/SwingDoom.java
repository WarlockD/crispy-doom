package awt;

import i.InputListener;

import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.event.InputEvent;

import javax.swing.JPanel;

import v.DoomVideoRenderer;
import doom.DoomMain;
import doom.event_t;

/** A simple Doom display & keyboard driver for AWT.
 *  Uses a Canvas for painting and implements some
 *  of the IVideo methods.
 * 
 *  Uses 8-bit buffered images and switchable IndexColorModels.
 *  
 *  It's really basic, but allows testing and user interaction.
 *  Heavily based on the original LinuxDoom X11 implementation, and
 *  is similar in goals: just a functional, reference implementation
 *  to build upon whatever fancy extensions you like.
 *  
 *  The only "hitch" is that this implementation expects to be 
 *  initialized upon a BufferedRenderer with multiple images per 
 *  screen buffer in order to perform the palette switching trick.
 *  
 *  The images themselves don't have to be "BufferedImage",
 *  and in theory it could be possible to use e.g. MemoryImageSource
 *  for the same purpose . Oh well.
 *    
 *  
 * 
 * @author Velktron
 *
 */
public abstract class SwingDoom<V> extends DoomFrame<V> {


    private static final long serialVersionUID = 3118508722502652276L;

		JPanel drawhere;

		/** Gimme some raw palette RGB data.
		 *  I will do the rest
		 *  
		 *  (hint: read this from the PLAYPAL
		 *   lump in the IWAD!!!).
		 * 
		 */
     
        @SuppressWarnings("unchecked")
		public SwingDoom(DoomMain<?,V> DM, DoomVideoRenderer<?,V> V) {
        	super(DM, V);
        	drawhere=new JPanel();
        	}
        
        Point center;
        Rectangle rect;

        
        public String processEvents(){
            StringBuffer tmp=new StringBuffer();
            event_t event;
            while ( (event=InputListener.nextEvent()) != null ) {
                tmp.append(event.type.ordinal()+"\n");
            }
            return tmp.toString();
        }
        
    	public void SetGamma(int level){
    		if (D) System.err.println("Setting gamma "+level);
    		V.setUsegamma(level);
    		screen=V.getCurrentScreen(); // Refresh screen after change.
    		RAWSCREEN=V.getScreen(DoomVideoRenderer.SCREEN_FG);
    	}
        
        // This stuff should NOT get through in keyboard events.
        protected final int UNACCEPTABLE_MODIFIERS=(int) (InputEvent.ALT_GRAPH_DOWN_MASK+
        										 InputEvent.META_DOWN_MASK+
        										 InputEvent.META_MASK+
        										 InputEvent.WINDOW_EVENT_MASK+
        										 InputEvent.WINDOW_FOCUS_EVENT_MASK);

    public static final class HiColor extends SwingDoom<short[]>{
        
        /**
         * 
         */
        private static final long serialVersionUID = 1L;

        public HiColor(DoomMain<?, short[]> DM, DoomVideoRenderer<?,short[]> V) {
            super(DM, V);
        }

        @Override
        public void ReadScreen(short[] scr) {
            System.arraycopy(this.RAWSCREEN, 0, scr, 0, RAWSCREEN.length);
            }
        
        @Override
        public void FinishUpdate() {
            int     tics;
            int     i;
            
            // draws little dots on the bottom of the screen
            /*if (true)
            {

            i = I.GetTime();
            tics = i - lasttic;
            lasttic = i;
            if (tics > 20) tics = 20;
            if (tics < 1) tics = 1;

            for (i=0 ; i<tics*2 ; i+=2)
                RAWSCREEN[ (SCREENHEIGHT-1)*SCREENWIDTH + i] = (byte) 0xff;
            for ( ; i<20*2 ; i+=2)
                RAWSCREEN[ (SCREENHEIGHT-1)*SCREENWIDTH + i] = 0x0;
            
            } */

            if (true)
            {

            i = TICK.GetTime();
            tics = i - lasttic;
            lasttic = i;
            if (tics<1) 
                frames++;
            else
            {
            //frames*=35;
            for (i=0 ; i<frames*2 ; i+=2)
                RAWSCREEN[ (height-1)*width + i] = (short) 0xffff;
            for ( ; i<20*2 ; i+=2)
                RAWSCREEN[ (height-1)*width + i] = 0x0;
            frames=0;
            }
            }

            this.update(null);
            //this.getInputContext().selectInputMethod(java.util.Locale.US);
            
        }
    }
    
    public static final class Indexed extends SwingDoom<byte[]>{
        
        /**
         * 
         */
        private static final long serialVersionUID = 1L;

        public Indexed(DoomMain<?,byte[]> DM, DoomVideoRenderer<?,byte[]> V) {
            super(DM, V);
        }

        @Override
        public void ReadScreen(byte[] scr) {
            System.arraycopy(this.RAWSCREEN, 0, scr, 0, RAWSCREEN.length);
            }
        
        @Override
        public void FinishUpdate() {
            int     tics;
            int     i;
            
            // draws little dots on the bottom of the screen
            /*if (true)
            {

            i = I.GetTime();
            tics = i - lasttic;
            lasttic = i;
            if (tics > 20) tics = 20;
            if (tics < 1) tics = 1;

            for (i=0 ; i<tics*2 ; i+=2)
                RAWSCREEN[ (SCREENHEIGHT-1)*SCREENWIDTH + i] = (byte) 0xff;
            for ( ; i<20*2 ; i+=2)
                RAWSCREEN[ (SCREENHEIGHT-1)*SCREENWIDTH + i] = 0x0;
            
            } */

            if (true)
            {

            i = TICK.GetTime();
            tics = i - lasttic;
            lasttic = i;
            if (tics<1) 
                frames++;
            else
            {
            //frames*=35;
            for (i=0 ; i<frames*2 ; i+=2)
                RAWSCREEN[ (height-1)*width + i] = (short) 0xffff;
            for ( ; i<20*2 ; i+=2)
                RAWSCREEN[ (height-1)*width + i] = 0x0;
            frames=0;
            }
            }

            this.update(null);
            //this.getInputContext().selectInputMethod(java.util.Locale.US);
            
        }
    }

    public static final class TrueColor extends SwingDoom<int[]>{
        
        /**
         * 
         */
        private static final long serialVersionUID = 1L;

        public TrueColor(DoomMain<?, int[]> DM, DoomVideoRenderer<?,int[]> V) {
            super(DM, V);
        }

        @Override
        public void ReadScreen(int[] scr) {
            System.arraycopy(this.RAWSCREEN, 0, scr, 0, RAWSCREEN.length);
            }
        
        @Override
        public void FinishUpdate() {
            int     tics;
            int     i;
            
            // draws little dots on the bottom of the screen
            /*if (true)
            {

            i = I.GetTime();
            tics = i - lasttic;
            lasttic = i;
            if (tics > 20) tics = 20;
            if (tics < 1) tics = 1;

            for (i=0 ; i<tics*2 ; i+=2)
                RAWSCREEN[ (SCREENHEIGHT-1)*SCREENWIDTH + i] = (byte) 0xff;
            for ( ; i<20*2 ; i+=2)
                RAWSCREEN[ (SCREENHEIGHT-1)*SCREENWIDTH + i] = 0x0;
            
            } */

            if (true)
            {

            i = TICK.GetTime();
            tics = i - lasttic;
            lasttic = i;
            if (tics<1) 
                frames++;
            else
            {
            //frames*=35;
            for (i=0 ; i<frames*2 ; i+=2)
                RAWSCREEN[ (height-1)*width + i] = (short) 0xffff;
            for ( ; i<20*2 ; i+=2)
                RAWSCREEN[ (height-1)*width + i] = 0x0;
            frames=0;
            }
            }

            this.update(null);
            //this.getInputContext().selectInputMethod(java.util.Locale.US);
            
        }
    }
}