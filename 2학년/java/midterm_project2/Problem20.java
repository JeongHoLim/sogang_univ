import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.select.Elements;

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
	
	private static Document doc = null;
	private static Elements titles;
	private static Elements authors;
	private static Elements prices;
	private static int len,i;
	private static String temp[];
	private static ArrayList<BookInfo> books;
	static ArrayList<BookInfo> readBooksJsoup(String src){
		
		books = new ArrayList<>();
		try {
			doc = Jsoup.connect(src).get();
		}catch(IOException e) {
			e.printStackTrace();
		}
		titles = doc.select(".product-info-title a[href]");
		temp = new String[titles.size()];
		authors = doc.select(".product-shelf-author.contributors");
		for(i=0;i<authors.size();i++) {	
			len = authors.eq(i).text().indexOf(",");
			if(len == -1) {
				temp[i] = new String(authors.eq(i).text().substring(3));
			}
			else {
				temp[i] = new String(authors.eq(i).text().substring(3,len));				
			}
		}
		prices = doc.select(".current a[href]");
		
		for(i=0;i<titles.size();i++) {
			books.add(new BookInfo(titles.eq(i).text(),temp[i],prices.eq(i).text(),i+1));
		}
		
		return books;
	}
	
}
public class Problem20 {

	public static void main(String[] args) {
		 ArrayList<BookInfo> books;
		 books = BookReader.readBooksJsoup("https://www.barnesandnoble.com/b/books/_/N-1fZ29Z8q8");
		 Collections.sort(books);
		 for(int i=0; i<books.size(); i++) {
			BookInfo book = books.get(i);
		 	System.out.println(book);
		 }

	}
}
