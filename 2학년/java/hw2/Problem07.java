import java.util.*;
interface IntSequenceStr{
	public boolean hasNext();
	public int next();
}
class BinarySequenceStr implements IntSequenceStr{
	
	private int n,tmp,i=-1;
	BinarySequenceStr(int num){
		n = num;
	}
	public boolean hasNext() {
		return i!=0;
	}
	public int cal() {
		tmp = n;
		for(int j=0;j<i-1;j++) {
			tmp /= 2;
		}
		if(n!=0) i--;
		return tmp%2;
	}
	public void find() {
		i=0;
		if(n!=0) {
			while(true) {
				if(n<Math.pow(2, i)) break;
				i++;
			}
		}
	}
	public int next() {
		if(i==-1) {
			find();
		}
		return cal(); 
	}
}
public class Problem07 {

	public static void main(String[] args) {
		Scanner in = new Scanner(System.in);
		System.out.print("Enter a positive integer: ");
		String str = in.nextLine();
		int num = Integer.parseInt(str);
		in.close();
		System.out.println("Integer: " + num);
		IntSequenceStr seq = new BinarySequenceStr(num);
		System.out.print("Binary number: ");
		while(seq.hasNext()) System.out.print(seq.next());
		System.out.println(" ");
	}

}
