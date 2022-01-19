
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.ConnectException;
import java.net.Socket;
import java.util.Scanner;

public class Client {
	static class ClientSender extends Thread{
		Socket socket;
		DataOutputStream out;
		String name;
		ClientSender(Socket socket,String name){
			this.socket = socket;
			try {
				out = new DataOutputStream(socket.getOutputStream());
				this.name = name;
			}catch(Exception e) {}
		}
		public void run() {
			Scanner scanner = new Scanner(System.in);
			try {
				if(out != null) {
					out.writeUTF(name);
				}
				while(out != null) {
					out.writeUTF(scanner.nextLine());
				}
			}catch(IOException e) {}
			scanner.close();
		}
	}
	static class ClientReceiver extends Thread{
		Socket socket;
		DataInputStream in;
		ClientReceiver(Socket socket){
			this.socket = socket;
			try {
				in = new DataInputStream(socket.getInputStream());
			}catch(IOException e) {}
		}
		public void run() {
			while(in != null) {
				try {
					String temp=in.readUTF();
					System.out.print(temp);
				}catch(IOException e) {}
			}
		}
	}
	public static void main(String[] args) {
		String id="";
		Scanner scanner;
		if(args.length != 2) {
			System.out.println("Please give the IP address and port number as arguments.");
			System.exit(0);
		}
		try {
			Thread sender;
			Thread receiver;
			String serverIp = args[0];
			Socket socket = new Socket(serverIp,Integer.parseInt(args[1]));
			while(true) {
				System.out.print("Enter usderID>> ");		
				scanner = new Scanner(System.in);
				id = scanner.nextLine();
				if(checkID(id) && id.length()>2){
					sender = new Thread(new ClientSender(socket,id));
					receiver = new Thread(new ClientReceiver(socket));
					break;
				}
				else {
					System.out.println("UserID must be a single word with lowercase alphabets and numbers.");	
				}
			}
			sender.start();
			receiver.start();
			
		}catch(ConnectException ce) {
			System.out.println("Connection establishment failed.");
			System.exit(0);
		}catch(Exception e) {e.printStackTrace();}
		
	}
	static boolean checkID(String id) {
		int i;

		for(i=0;i<id.length();i++) {
			if((id.charAt(i) < 'a' || id.charAt(i)>'z') && (id.charAt(i) < '0' || id.charAt(i)>'9')) {
				return false;
			}
		}
		if(i==0) return false;
		return true;
		
	}
}

