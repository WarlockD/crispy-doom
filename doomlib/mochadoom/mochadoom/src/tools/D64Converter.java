package tools;

import java.io.BufferedOutputStream;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Arrays;
import java.util.HashMap;

import rr.line_t;
import data.maplinedef_t;
import data.mapsector_t;
import data.mapsidedef_t;
import data.mapthing_t;
import data.mapvertex_t;
import doom64.MapLights;
import doom64.MapLinedef;
import doom64.MapSector;
import doom64.MapSidedef;
import doom64.MapThing;
import doom64.MapVertex;
import utils.C2JUtils;
import w.*;


public class D64Converter {	    
	    
	    static final String[] MAPS={"MAP01" ,"MAP02","MAP03","MAP04","MAP05",
	    							"MAP06","MAP07","MAP08","MAP09","MAP10",
	    							"MAP11","MAP12","MAP13","MAP14","MAP15",
	    							"MAP16","MAP17","MAP18","MAP19","MAP20",
	    							"MAP21","MAP22","MAP23","MAP24","MAP25",
	    							"MAP26","MAP27","MAP28","MAP29","MAP30",
	    							"MAP31","MAP32","MAP33"};
	    static final String DOOM64="d://doom//iwads//doom64.wad";
	    
	    static final String[] COPY_LUMPS_1={"SEGS","SSECTORS","NODES"};
	    static final String[] COPY_LUMPS_2={"REJECT","BLOCKMAP"};
	    static final String VOID_TEXTURE="-";
	    WadLoader W,WAD64;
	    String[] TEXTURES,FLATS;
	    HashMap<Integer,String> TexHash,FlatHash;

	    /* ResourceManager::getTextureHash
	     * Returns the Doom64 hash of a given texture name, computed using
	     * the same hash algorithm as Doom64 EX itself
	     *******************************************************************/
	    public static int getTextureHash(String name)
	    {
	    	char[] str=new char[8];
	    	
	    	Arrays.fill(str, (char)0);
	    	
	    	 for (int c = 0; c < name.length() && c < 8; c++)
	    		str[c] = name.charAt(c);

	    	long hash = 1315423911;
	    	for(int i = 0; i < 8 && str[i] != '\0'; ++i){
	    		hash ^= ((hash << 5)+ Character.toUpperCase(str[i]) + (hash >> 2));
	    		hash&=0x00FFFFFFFFL;
	    	}
	    	
	    	hash %= 65536;
	    	return (int) hash;
	    }
	    
	    public mapsector_t[] ConvertSectors(int lump, MapLights[] lights) throws IOException
	    {
	        MapSector[] data;
	        mapsector_t[] sectors;
	        MapSector    d64;
	        mapsector_t  doom;
	        
	        
	        int numsectors = WAD64.LumpLength (lump) / MapSector.sizeOf();
	        
	       
	        // Read "mapsectors"
	        data=WAD64.CacheLumpNumIntoArray(lump,numsectors,MapSector.class);
	        sectors = C2JUtils.createArrayOfObjects(mapsector_t.class,numsectors);
	        
	        
	        for (int i=0 ; i<numsectors ; i++)
	        {
	        	d64 = data[i];
	        	doom = sectors[i];
	        	doom.floorheight =d64.floorheight;
	        	doom.ceilingheight = d64.ceilingheight;
	        	doom.floorpic=FlatHash.get(new Integer(d64.floorpic));
	        	doom.ceilingpic=FlatHash.get(new Integer(d64.ceilingpic));
	        	
	        	if (doom.floorpic==null) doom.floorpic=TexHash.get(new Integer(d64.floorpic));
	        	if (doom.floorpic==null) System.err.printf("Warning: Sector %d floorpic hash %d has null mapping!\n",i,(int)d64.floorpic);

	        	if (doom.ceilingpic==null) doom.ceilingpic=TexHash.get(new Integer(d64.ceilingpic));
	        	if (doom.ceilingpic==null) System.err.printf("Warning: Sector %d ceilingpic hash %d has null mapping!\n",i,(int)d64.ceilingpic);
	        	
	        	if(doom.floorpic.equals("?")) doom.floorpic=VOID_TEXTURE;
	        	if(doom.ceilingpic.equals("?")) doom.ceilingpic=VOID_TEXTURE;
	        	
	        	// System.out.printf("Sector %d mapped to %s %s\n",i,dooms.floorpic,dooms.ceilingpic);
	        	doom.lightlevel=lights[d64.thingcolor].greyscaleLightLevel();
	        	//dooms.floorpic = FLATS[d64s.floorpic];
	        	//dooms.ceilingpic = FLATS[d64s.ceilingpic];
	        	//dooms.lightlevel = ms.lightlevel;
	        	doom.special = d64.special;
	        	doom.tag = d64.tag;
	        }
	        
	        return sectors;

	    }
	    
	    /**
	     * P_LoadThings
	   * @throws IOException 
	     */
	    public mapthing_t[] ConvertThings(int lump) throws IOException
	    {
	        MapThing[]       data;
	        mapthing_t[] things;
	        MapThing d64thing;
	        mapthing_t     mt;
	        int         numthings;
     
	        
	        numthings = WAD64.LumpLength (lump) / MapThing.sizeOf();
	        
	        data=WAD64.CacheLumpNumIntoArray(lump,numthings,MapThing.class);
	        things = C2JUtils.createArrayOfObjects(mapthing_t.class,numthings);
	        
	        for (int i=0 ; i<numthings ; i++)   {
	        	mt=things[i];
	        	d64thing=data[i];
	        	mt.x=d64thing.x;
	        	mt.y=d64thing.y;
	        	mt.angle=d64thing.angle;
	        	mt.options=d64thing.flags;
	        	mt.type=d64thing.type;
	        }        
	        
	        return things;
	        
	    }

	    public maplinedef_t[] ConvertLinedefs(int lump) throws IOException
	    {
	        MapLinedef[]       data;
	        maplinedef_t[]       lines;
	        MapLinedef d64l;
	        maplinedef_t   mld;
	        
	        int numlines = WAD64.LumpLength (lump) / MapLinedef.sizeOf();
	        lines = C2JUtils.createArrayOfObjects(maplinedef_t.class,numlines);

	        // read "maplinedefs"
	        data = WAD64.CacheLumpNumIntoArray(lump,numlines,MapLinedef.class);
	        
	        for (int i=0 ; i<numlines ; i++)
	        {
	            d64l=data[i];
	        	mld=lines[i];      	        
	        	
	        	mld.v1=(char) d64l.v1;
	        	mld.v2=(char) d64l.v2;
	        	// TODO: adapt flags
	        	mld.flags = (short) (d64l.flags&0xFFFF);
	        	mld.special = (short) (d64l.special&0xFF);
	        	mld.tag = d64l.tag;
	        	mld.sidenum[0]=(char) d64l.left;
	        	mld.sidenum[1]=(char) d64l.right;
	        	
	        }
	        
	        return lines;	        

	     }
	
	    public mapsidedef_t[] ConvertSidedefs(int lump) throws IOException
	    {
	        MapSidedef[] data;
	        mapsidedef_t[] sides;
	        MapSidedef    d64;
	        mapsidedef_t  doom;	        
	        
	        int numsides = WAD64.LumpLength (lump) / MapSidedef.sizeOf();
	        
	       
	        // Read "mapsectors"
	        data=WAD64.CacheLumpNumIntoArray(lump,numsides,MapSidedef.class);
	        sides = C2JUtils.createArrayOfObjects(mapsidedef_t.class,numsides);
	        
	        
	        for (int i=0 ; i<numsides ; i++)
	        {
	        	d64 = data[i];
	        	doom = sides[i];
	        	doom.textureoffset=d64.xoffset;
	        	doom.rowoffset=d64.yoffset;
	        	doom.sector=d64.sector;
	        	doom.bottomtexture=TexHash.get(new Integer(d64.bottomtexture));
	        	doom.midtexture=TexHash.get(new Integer(d64.midtexture));
	        	doom.toptexture=TexHash.get(new Integer(d64.toptexture));
	        		
        		if (doom.bottomtexture==null)	doom.bottomtexture=FlatHash.get(new Integer(d64.bottomtexture));
	        	if (doom.bottomtexture==null) System.err.printf("Warning: Sidedef %d bottomtexture hash %d has null mapping!\n",i,(int)d64.bottomtexture);
	        	
	        	if (doom.midtexture==null) doom.midtexture=FlatHash.get(new Integer(d64.midtexture));
	        	if (doom.midtexture==null) System.err.printf("Warning: Sidedef %d midtexture hash %d has null mapping!\n",i,(int)d64.midtexture);
	        	
	        	if (doom.toptexture==null) doom.toptexture=FlatHash.get(new Integer(d64.toptexture));
	        	if (doom.toptexture==null) System.err.printf("Warning: Sidedef %d toptexture hash %d has null mapping!\n",i,(int)d64.toptexture);
	        	
	        	if(doom.bottomtexture.equals("?")) doom.bottomtexture=VOID_TEXTURE;
	        	if(doom.midtexture.equals("?")) doom.midtexture=VOID_TEXTURE;
	        	if(doom.toptexture.equals("?")) doom.toptexture=VOID_TEXTURE;
	        	
	        	// System.out.printf("Sidedef %d mapped to %s %s %s\n",i,doom.bottomtexture,doom.midtexture,doom.toptexture);
	        }
	        
	        return sides;

	    }
	    
	    public mapvertex_t[] ConvertVertexes(int lump) throws IOException
	    {
	        MapVertex[] data;
	        mapvertex_t[] vertexes;
	        MapVertex    d64;
	        mapvertex_t  doom;	        
	        
	        int numvertexes = WAD64.LumpLength (lump) / MapVertex.sizeOf();	        
	       
	        data=WAD64.CacheLumpNumIntoArray(lump,numvertexes,MapVertex.class);
	        vertexes = C2JUtils.createArrayOfObjects(mapvertex_t.class,numvertexes);	        
	        
	        for (int i=0 ; i<numvertexes ; i++)
	        {
	        	d64 = data[i];
	        	doom = vertexes[i];
	        	doom.x= (short) (d64.x>>16);
	        	doom.y= (short) (d64.y>>16);
	        }
	        
	        return vertexes;

	    }	    	    
	    
	    
		private MapLights[] ConvertLights(int lump) {
			MapLights[] lights;
			MapLights[] data;
	        
	        int numlights = WAD64.LumpLength (lump) / MapLights.sizeOf();
	        
	        // First 256 lights are auto-generated
	        lights=new MapLights[256+numlights];
	        
	        for (int i=0;i<256;i++){
	        	lights[i]=new MapLights((short) i);	        	
	        }
	        	       
	        // Read extra lights
	        data=WAD64.CacheLumpNumIntoArray(lump,numlights,MapLights.class);
	        
	        for (int i=0;i<numlights;i++)	lights[256+i]=data[i];
	        return lights;
		}
	    
	    
	    public static void main(String[] argv) throws Exception {	    	
	    	D64Converter conv=new D64Converter();
	    	conv.gogogo();
	    }
	    
	    public void gogogo() throws Exception{	    	
		    DataInputStream dis=new DataInputStream(new FileInputStream(DOOM64));
	    
		    W=new WadLoader(DOOM64);
		    
		    System.out.println(W.numlumps);
		    
		    int T_START=W.GetNumForName("T_START");
		    int T_END=W.GetNumForName("T_END");
		    
	    
		    // Find "?" textures. The first texture and the first flat will both be "?"
		    
		    int[] suck_markers=new int[2];
		    int mark=0;
		    
		    for (int lump=T_START+1;lump<T_END;lump++){
		    	if (W.GetNameForNum(lump).equals("?")) suck_markers[mark++]=lump;
		    	if (mark>1) break;
		    }
		    
		    // Can't separate between flats and textures, not enough "?"
		    if (mark<2) return;
		    
		    TEXTURES=new String[suck_markers[1]-suck_markers[0]];
		    FLATS=new String[T_END-suck_markers[1]];
		    
		    TexHash=new HashMap<Integer,String>();
		    FlatHash=new HashMap<Integer,String>();
		    
		    
		    int k=0;
		    for (int lump=suck_markers[0];lump<suck_markers[1];lump++,k++){
		    	TEXTURES[k]=W.GetNameForNum(lump);
		    	System.out.printf("Texture %d %s hash %d\n",k,TEXTURES[k],getTextureHash(TEXTURES[k]));
		    	TexHash.put(getTextureHash(TEXTURES[k]),TEXTURES[k]);
		    }
		    
		    k=0;
		    for (int lump=suck_markers[1];lump<T_END;lump++,k++){
		    	FLATS[k]=W.GetNameForNum(lump);
		    	System.out.printf("Flat %d %s hash %d\n",k,FLATS[k],getTextureHash(FLATS[k]));
		    	FlatHash.put(getTextureHash(FLATS[k]),FLATS[k]);
		    }
		    
		    WadBuilder WB=new WadBuilder("Doom64conv.wad", "PWAD");
		    WB.start();
		    
		    for (String map: MAPS){
		    		System.err.println(map);
	    	
		            byte[] mapdata=W.CacheLumpNameAsRawBytes(map,0);
		            File f=new File(String.format("%s.d64.wad",map));
		            BufferedOutputStream bos=new BufferedOutputStream(new FileOutputStream(f));
		            bos.write(mapdata);
		            bos.close();
		            
		            WAD64=new WadLoader(f.getName());
		            System.out.printf("%s %d\n",f.getName(),WAD64.numlumps);

		            // We have no use for the LEAFS and MACROS nodes, but we do have some use for the LIGHTS node,
		            // since it's the only way to deduct some lighting information.
		            
		            int lump=WAD64.GetNumForName("LIGHTS");
		            MapLights[] lights=ConvertLights(lump);
		            
		            lump=WAD64.GetNumForName("THINGS");
		            mapthing_t[] things=ConvertThings(lump);

		            lump=WAD64.GetNumForName("LINEDEFS");
		            maplinedef_t[] lines=ConvertLinedefs(lump);		            
		            
		            lump=WAD64.GetNumForName("SIDEDEFS");
		            mapsidedef_t[] sides=ConvertSidedefs(lump);
		            
		            ProcessLinesSides(lines,sides);
	            
		            lump=WAD64.GetNumForName("SECTORS");
		            mapsector_t[] sectors=ConvertSectors(lump,lights);
		            
		            lump=WAD64.GetNumForName("VERTEXES");
		            mapvertex_t[] vertexes=ConvertVertexes(lump);
	            
		            WB.add(map);
		            WB.add("THINGS", things);
		            WB.add("LINEDEFS", lines);
		            WB.add("SIDEDEFS", sides);
		            WB.add("VERTEXES", vertexes);
		            
		            // SEGS, SSECTORS and NODES are the same as vanilla
		            WB.add(WAD64,COPY_LUMPS_1);
		            
		            WB.add("SECTORS", sectors);
		            
		            // REJECT and BLOCKMAP are the same as vanilla...
		            WB.add(WAD64,COPY_LUMPS_2);	            
		            }
		    
            	WB.close();
	    }

		private void ProcessLinesSides(maplinedef_t[] lines,
				mapsidedef_t[] sides) {

			for (int k=0;k<lines.length;k++){
				//  Samuel: there's a flag in D64 maps that is "render mid-texture"
				// if that flag is not set but there is a midtexture present, it should be nullified to no texture
				// note that the nullification should only occur on two-sided lines, one-sided lines should be left alone in that regard
				//
				
				if (C2JUtils.flags(lines[k].flags,line_t.ML_TWOSIDED) &&
						!C2JUtils.flags(lines[k].flags,MapLinedef.D64_ML_RENDERMIDDLE)) {
					int front=lines[k].sidenum[0];
					int back=lines[k].sidenum[1];
						
					if(front!=0xFFFF && !sides[front].midtexture.equals(VOID_TEXTURE)){
	        			System.err.printf("Linedef %d Sidedef %d (front) Flags %x Mid %s =>",k,front,lines[k].flags,sides[front].midtexture);
	        			sides[front].midtexture=VOID_TEXTURE;
	        			System.err.printf(" %s\n",sides[front].midtexture);
	        		}
					
					if(back!=0xFFFF && !sides[back].midtexture.equals(VOID_TEXTURE)){
	        			System.err.printf("Linedef %d Sidedef %d (front) Flags %x Mid %s =>",k,back,lines[k].flags,sides[back].midtexture);
	        			sides[back].midtexture=VOID_TEXTURE;
	        			System.err.printf(" %s\n",sides[back].midtexture);
	        		}

					
	        	}

			}
		
		}

}
