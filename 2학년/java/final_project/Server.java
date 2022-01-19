import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Map.Entry;
import java.util.TreeMap;

class Book implements Comparable<Book>{
	String author;
	String title;
	Book(String title, String author){
		this.author = author;
		this.title = title;
	}
	public String toString() { return title + "" + author;}
	public int compareTo(Book o) {
		return this.title.compareToIgnoreCase(o.title);
	}
}

class BookReader{
	
	static TreeMap<Book,String> library = new TreeMap<>();
	static boolean fileToBox(String file) {
		String arr[];
		String line;
		BufferedReader br = null;
		try {
			br = new BufferedReader(new FileReader(file));
			while(true) {
				line = br.readLine();
				if(line == null) break;
				arr = line.split("\t");
				library.put(new Book(arr[0],arr[1]),arr[2]);
			}
			Server.update();
		}catch(FileNotFoundException e) {
			System.out.println("Input file not found.");
			return false;
		}catch(IOException e) {
			e.printStackTrace();
		}
		try{
			br.close();
		}catch(IOException e) {
			e.printStackTrace();
		}	
		return true;
	}
}

public class Server {

	HashMap<String,DataOutputStream> clients;
	public static void main(String[] args) {
		if(args.length != 1) {
			System.out.println("Please give the port number as an argument.");
			System.exit(0);
		}
		BookReader.fileToBox("books.txt");
		new Server().start(Integer.parseInt(args[0]));
	}
	Server(){
		clients = new HashMap<>();
		Collections.synchronizedMap(clients);
	}
	static void update() {
		FileWriter fw;
		BufferedWriter bw = null; 
		try {
			fw = new FileWriter("books.txt");
			bw = new BufferedWriter(fw);
			Iterator<Map.Entry<Book,String>> entries = BookReader.library.entrySet().iterator();
			while(entries.hasNext()){
				Entry<Book,String> entry = (Entry<Book,String>)entries.next();
				bw.write(entry.getKey().title + "\t" + entry.getKey().author + "\t"+entry.getValue()+"\n");
			}
		
		}catch(IOException e) {
			e.printStackTrace();
		}
		try {
			bw.close();
		}catch(IOException e) { e.printStackTrace();}
	}
	public void start(int port) {
		ServerSocket serverSocket = null;
		Socket socket = null;
		try {
			serverSocket = new ServerSocket(port);
			System.out.println("server has started.");
			while(true) {
				socket = serverSocket.accept();
				ServerReceiver thread = new ServerReceiver(socket);
				thread.start();
			}
		}catch(Exception e) { e.printStackTrace();}
	}
	void sendMsg(String msg,String client){
	
		try {
			DataOutputStream out = (DataOutputStream) clients.get(client);
			out.writeUTF(msg);
		}catch(IOException e) { e.printStackTrace();}
		
	}
	class ServerReceiver extends Thread{
		Socket socket;
		DataInputStream in;
		DataOutputStream out;
		String request= "";
		Book temp;
		ServerReceiver (Socket socket){
			this.socket = socket;
			try {
				in = new DataInputStream(socket.getInputStream());
				out = new DataOutputStream(socket.getOutputStream());
			}catch(IOException e) { e.printStackTrace();}
		}
		
		void addBook(String name) {
			String title ="";
			String author = "";
			int flg=1;
			try {
				sendMsg("add-title> ",name);
				request = in.readUTF();
				if(request.trim().equals("")) return; 
				sendMsg("add-author> ",name);
				title = request;
				request = in.readUTF();
				if(request.trim().equals("")) return;
				author = request;
				Iterator<Book> keys = BookReader.library.keySet().iterator();
				while(keys.hasNext()) {
					String key = keys.next().title;
					if(key.equalsIgnoreCase(title)){
						flg=0;
						break;
					}
				}
				if(flg==1) {
					BookReader.library.put(new Book(title,author),"-");
					sendMsg("A new Book added to the list.\n",name);
					update();
				}else {
					sendMsg("The book already exists in the list.\n",name);
				}
				return;
			}catch(IOException e) { e.printStackTrace();}
		}
		void borrow(String name) {
			String title;
			String 	cmp;
			try{
				sendMsg("borrow-title> ",name);
				request = in.readUTF();
				if(request.trim().equals("")) return;
				title = request;
				Iterator<Map.Entry<Book,String>> entries = BookReader.library.entrySet().iterator();
				while(entries.hasNext()){
					Entry<Book,String> entry = (Entry<Book,String>)entries.next();
					cmp = entry.getKey().title;
					if(cmp.equalsIgnoreCase(title)) {
						if(entry.getValue().equals("-")) {
							 BookReader.library.put(new Book(entry.getKey().title,entry.getKey().author),name);
							 sendMsg("You borrowed a book. - "+entry.getKey().title+"\n",name);
							 update();
						}
						else {
							sendMsg("The book is not available.\n",name);
						}
						return;
					}
				}
				sendMsg("The book is not available.\n",name);
			}catch(IOException e) { e.printStackTrace();}
			
		}
		void returnBook(String name) {
			String title;
			String cmp;
			try{
				sendMsg("return-title> ",name);
				request = in.readUTF();
				if(request.trim().equals("")) return;
				title = request;
				Iterator<Map.Entry<Book,String>> entries = BookReader.library.entrySet().iterator();
				while(entries.hasNext()){
					Entry<Book,String> entry = (Entry<Book,String>)entries.next();
					cmp = entry.getKey().title;
					if(cmp.equalsIgnoreCase(title)) {
						if(entry.getValue().equals(name)) {
							 BookReader.library.put(new Book(entry.getKey().title,entry.getKey().author),"-");
							 sendMsg("You returned a book. - "+entry.getKey().title+"\n",name);
							 update();
						}
						else {
							sendMsg("You did not borrow the book.\n",name);
						}
						return;
					}
				}				
				sendMsg("You did not borrow the book.\n",name);
			}catch(IOException e) { e.printStackTrace();}
			
		}
		void info(String name) {
			String cmp;
			int num=0;
			ArrayList<String>arr = new ArrayList<>();
			Iterator<Map.Entry<Book,String>> entries = BookReader.library.entrySet().iterator();
			while(entries.hasNext()){
				Entry<Book,String> entry = (Entry<Book,String>)entries.next();
				cmp = entry.getValue();
				if(cmp.equalsIgnoreCase(name)) {
					num++;
					arr.add(num +". " +entry.getKey().title + ", " + entry.getKey().author + "\n");
				}
			}				
			sendMsg("You are currently borrowing "+ num + " books: \n",name);
			Iterator<String>it = arr.iterator();

			while (it.hasNext()) {
			    sendMsg(it.next(),name);
			}
		}
		String getInput(String name) {
			try{
				sendMsg("search-string> ",name);
				request = in.readUTF();
				if(request.length()==0) {
					return null;
				}
				else if(request.length()<3) {
					sendMsg("Search string must be longer than 2 characters.\n",name);
				}
				
			}catch(IOException e) { e.printStackTrace();}
			return request;
		}
		void search(String name) {
			int num=0;
			String cmp;
			String seq="";
			while(in != null) {
				seq = getInput(name);
				if(seq == null) return;
				if(seq.length()>2) {
					ArrayList<String>arr = new ArrayList<>();
					Iterator<Map.Entry<Book,String>> entries = BookReader.library.entrySet().iterator();
					while(entries.hasNext()){
						Entry<Book,String> entry = (Entry<Book,String>)entries.next();
						cmp = entry.getKey().title + " " + entry.getKey().author ;
						if(cmp.contains(seq)) {
							num++;
							arr.add(num +". " +entry.getKey().title + ", " + entry.getKey().author + "\n");
						}
					}
					sendMsg("Your search matched "+num+" results.\n",name);
					Iterator<String>it = arr.iterator();
					while (it.hasNext()) {
					    sendMsg(it.next(),name);
					}
					return;
				}
			
			}
			
		}
		void commands(String name) {
			sendMsg("[available commands]\n",name);
			sendMsg("add: add a new book to the list of books.\n",name);
			sendMsg("borrow: borrow a book from the library.\n",name);
			sendMsg("return: return a book to the library.\n",name);
			sendMsg("info: show list of books I am currently borrowing.\n",name);
			sendMsg("search: search for books.\n",name);
			return;
		}
		public void run() {
			String name = "";
			try {
				name = in.readUTF();
				clients.put(name, out);
				sendMsg("Hello "+name + "!\n",name);
				
				while(in != null) {
					sendMsg(name + ">> ",name);		
					request = in.readUTF();
					if(request.equals("add")) {
						addBook(name);
					}
					else if(request.equals("borrow")) {
						borrow(name);
					}
					else if(request.equals("return")) {
						returnBook(name);
						
					}else if(request.equals("info") ) {
						info(name);
					}
					else if(request.equals("search")) {
						search(name);
					}
					else {
						commands(name);
					}
				}

			}catch(IOException e) { e.printStackTrace();
			}finally {
				clients.remove(name);
				System.out.println("["+socket.getInetAddress() + ":" + socket.getPort()+"]"+ " has disconnected.");
				System.out.println("current number of users: " + clients.size());
			}
		}
	}
	
}