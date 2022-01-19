interface IntSequence{
	public boolean hasNext();
	public int next();
}
class FibonacciSequence implements IntSequence{
	public int cal(int n) {
		if(n==0) return 0;
		else if(n==1) return 1;
		else return cal(n-1)+cal(n-2);
		
	}
	private int n=0;
	public boolean hasNext() {
		if(n==20) return false;
		else return true;
		
	}
	public int next() {
		return cal(n++);
	}
}

public class Problem06 {

	public static void main(String[] args) {
		IntSequence seq = new FibonacciSequence();
		for(int i=0;i<20;i++) {
			if(seq.hasNext() == false) break;
			System.out.print(seq.next() + " ");
		}
		System.out.println(" ");
	}

}

