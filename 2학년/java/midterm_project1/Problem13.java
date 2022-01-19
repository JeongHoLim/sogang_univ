import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;

class Text{
	
	private byte[] b = new byte[1024];
	private int cnt[] = new int[26];
	private int rv,i;
	private FileInputStream input;
	boolean readTextFromFile(String file){
		try {
			input = new FileInputStream(file);
			while(true) {
				rv = input.read(b);
				count(rv,b);
				if(rv == -1) break;
			}
		}catch(FileNotFoundException e) {
			System.out.println("Input file not found.");
			return false;
		}catch(IOException e) {
			e.printStackTrace();
		}
		try{
			input.close();
		}catch(IOException e) {
			e.printStackTrace();
		}

		return true;
	}
	boolean check(byte x) {
		return ((x>='A' && x<='Z') || (x>='a'&&x<='z'));
		
	}
	void count(int rv,byte b[]) {
		for(i=0;i<rv;i++) {
			if(check(b[i])) {
				if(b[i]>='a') cnt[b[i]-'a']++;
				else if(b[i]>='A') cnt[b[i]-'A']++;
			}
		}
	}
	int countChar(char x) {
		return cnt[x-'a'];
	}
	
}
public class Problem13 {

	public static void main(String[] args){
		Text t = new Text();
		if(t.readTextFromFile("input_prob13.txt")) {
			for(char c = 'a';c<='z';c++) {
				System.out.println(c + ": " + t.countChar(c));
			}
		}
	}

}
