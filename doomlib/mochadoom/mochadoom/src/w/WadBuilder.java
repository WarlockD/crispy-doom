package w;

import java.io.DataOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/** A very simple WAD building utility class, just enough to lay some organized lumps on disk. Not particularly optimized 
 * 
 *  It works simply by adding IWritableDoomObjects, IPackableDoomObjects and raw byte[] data to a list. Then, when the time 
 *  to write everything to disk comes, a header, index table etc. are created as well.
 * 
 * */

public class WadBuilder {
	
	protected File file;
	protected wadheader_t header;
	protected List<IWritableDoomObject> lumps;
	protected DataOutputStream dos;
	protected FileOutputStream fos;
	protected int indexpos,totalsize;
	
	public WadBuilder(String file,String type) {
		this.header=new wadheader_t();
		this.lumps=new ArrayList<IWritableDoomObject>();
		this.header.type=type;
		this.file=new File(file);
		this.indexpos=0;
	}
	
	public void start() throws IOException {
		if (file.exists()) file.delete();
		fos=new FileOutputStream(file,false);
		dos=new DataOutputStream(fos);		
		// This header is only temporary, until we know how many lumps and data we've actually written.
		header.write(dos);
	}

	/** Add a zero-sized marker
	 * 
	 * @param lumpname
	 * @throws IOException
	 */
	public void add(String lumpname) throws IOException{		
		filelump_t lump=new filelump_t(lumpname,0,dos.size());
		lumps.add(lump);
		}
	
	/** Add and write immediately a raw byte[] bunch of data.
	 * 
	 * @param lumpname
	 * @param data
	 * @throws IOException
	 */
	
	public void add(String lumpname, byte[] data) throws IOException{		
		filelump_t lump=new filelump_t(lumpname,data.length,dos.size());
		lumps.add(lump);
		dos.write(data);
		}
	
	/** Add and write immediately an object implementing the IWritableDoomObject interface.
	 * 
	 * @param lumpname
	 * @param stuff
	 * @throws IOException
	 */
	public void add(String lumpname, IWritableDoomObject stuff) throws IOException{		
		
		long pos=dos.size(); // Save current stream position
		stuff.write(dos);
		
		// The new lump can be created ONLY after the write
		filelump_t lump=new filelump_t(lumpname,dos.size()-pos,pos);
		lumps.add(lump);
		
		}
	
	/** Add and write immediately an array of objects implementing the IWritableDoomObject interface.
	 *  Objects will be written contiguously.
	 * 
	 * @param lumpname
	 * @param stuff
	 * @throws IOException
	 */
	
	public void add(String lumpname, IWritableDoomObject stuff[]) throws IOException{		
		
		long pos=dos.size(); // Save current stream position
		
		DoomIO.writeListOfObjects(dos, stuff,stuff.length);
		
		// The new lump can be created ONLY after the write
		filelump_t lump=new filelump_t(lumpname,dos.size()-pos,pos);
		lumps.add(lump);		
		}
	
	/** When called, the WAD index will be written to the file, the header will be
	 *  updated for the total number of entries and index position, and the file closed.
	 * 
	 * @throws IOException
	 */
	
	public void close() throws IOException{
		
		this.indexpos=dos.size();
		// Time to write the index
		DoomIO.writeListOfObjects(dos, this.lumps, this.lumps.size());
		this.totalsize=dos.size();
		
		// Amend header
		this.header.numentries=lumps.size();
		this.header.tablepos=this.indexpos;
		
		// Write amended header
		fos.getChannel().position(0);
		header.write(dos);
		
		// Close streams
		dos.close();
		fos.close();
		}
	
	/** Directly copy a bunch of lumps from an IWadLoader object to this WAD file.
	 * 
	 * @param W
	 * @param lumplist
	 * @throws IOException
	 */
	
    public void add(IWadLoader W,String[] lumplist) throws IOException{
    	byte[] data;
    	
    	for (String lumpname: lumplist){
    		data=W.CacheLumpNameAsRawBytes(lumpname, 0);
    		this.add(lumpname,data);
    	}
    }
	
}
