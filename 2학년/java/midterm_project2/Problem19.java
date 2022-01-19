/*
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.URL;
import java.util.ArrayList;
import java.util.Collections;

class BookInfo implements Comparable<BookInfo>{
	private String title;
	private String author;
	private String price;
	private int rank;
	BookInfo(String title,String author,String price,int rank){
		this.title = title; this.author = author; this.price = price; this.rank = rank;
	}
	public String toString() { return "#"+this.rank+" "+this.title +", " + this.author +", " + this.price;
	}
	int getRank() { return this.rank;}
	@Override
	public int compareTo(BookInfo o) {
		if(this.rank > o.getRank()) { return -1;}
		else return 1;
	}
	
}
	
class BookReader{
	private static ArrayList<String> lines; 
	private static ArrayList<BookInfo> books; 
	private static URL url;
	private static BufferedReader input;
	private static String line;
	private static String author;
	private static String title;
	private static String price;
	private static int rank;
	private static int status;
	private static int start, end;
	private static int i;
	static ArrayList<BookInfo> readBooks(String src){
		lines = new ArrayList<>();
		books = new ArrayList<BookInfo>();
		url = null;
		input = null;
		line = "";
		author =null;
		title=null;
		price=null;
		rank=1;
		try{
			url = new URL(src);
			input= new BufferedReader(new InputStreamReader(url.openStream()));
			while((line = input.readLine())!=null) {
				if(line.trim().length()>0) lines.add(line);
			}
			input.close();
		}catch(Exception e){ 
			e.printStackTrace();
		}
		status = 0;
		for(i=0;i<lines.size();i++) {
			String l = lines.get(i);
			if(status == 0) {
				if(l.contains("<h3 class=\"product-info-title\">")) {
					status=1;
				}
			}
			else if(status == 1) {
				if(l.contains("<a title=\"\" class=\" \" onclick=\"set_cookie('txl__")) {
					start = l.indexOf("\">") + "\">".length();
					end = l.indexOf("</a>");
					title = l.substring(start,end);
					status = 2;
				}
			}
			else if(status == 2) {
				if(l.contains("<div class=\"product-shelf-author contributors\">")) {
					end = l.indexOf("</a>");					
					String temp = l.substring(0,end);
					start = temp.lastIndexOf("\">");
					author = l.substring(start+2, end);
					status = 3;
				}
			}
			else if(status == 3) {
				if(l.contains("<a title=\"\" class=\" current link\"")) {
					start = l.lastIndexOf("\">");
					end = l.indexOf("</a>");
					price = l.substring(start+2, end);
					books.add(new BookInfo(title,author,price,rank));
					rank++;
					status=0;
				}
				
			}
		}
		return books;
	}
}
public class Problem19 {

	public static void main(String[] args) {
		ArrayList<BookInfo> books;
		books = BookReader.readBooks("https://www.barnesandnoble.com/b/books/_/N-1fZ29Z8q8");
		Collections.sort(books);
		for(int i=0; i<books.size(); i++) {
			BookInfo book = books.get(i);
			System.out.println(book);
		}

	}
}
*/