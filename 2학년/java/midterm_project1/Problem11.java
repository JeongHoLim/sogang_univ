class PalindromeChecker{

	private static int i,flg=1;
	private static char arr[];
	private static String arr2;
	static void check(String s) {
		arr=s.toCharArray();
		for(i=0;i<s.length()/2;i++) {
			if(arr[i] != arr[s.length()-i-1]) flg=0;
		}
		System.out.print(s);
		print(flg);
		flg=1;
		
	}
	static void check(int num) {
		arr2 = String.valueOf(num);
		for(i=0;i<arr2.length()/2;i++) {
			if(arr2.charAt(i) != arr2.charAt(arr2.length()-i-1)) flg=0;
		}
		System.out.print(num);
		print(flg);		
		flg=1;
	}
	static void print(int flag) {
		if(flg==0) System.out.println(" is not a palindrome.");
		else System.out.println(" is a palindrome.");		
	}
}
public class Problem11 {

	public static void main(String[] args) {
		PalindromeChecker.check("abcde");
		PalindromeChecker.check("abcba");
		PalindromeChecker.check(1234);
		PalindromeChecker.check(12321);
	}

}
