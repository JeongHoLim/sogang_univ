
class SubsequenceChecker{
	
	private static int i,j;
	private static int cnt[]; 
	static void check(String str1,String str2) {
		
		if(str2.length()==0) {
			System.out.println(str2 +" is not a subsequence of " + str1 );
			return;
		}
		cnt = new int[str2.length()];
		for(i=0,j=0;i<str1.length();i++) {
			if(str2.charAt(j) == str1.charAt(i)) {
				cnt[j] = i;
				j++;
				if(j==str2.length()) break;
			}
		}
		if(j==str2.length()) {
			System.out.println(str2 +" is a subsequence of " + str1 );
			for(i=0;i<j;i++)
				System.out.print(cnt[i] + " ");
			System.out.println();
		}
		else {
			System.out.println(str2 +" is not a subsequence of " + str1 );
		}
	}
	
}
public class Problem12 {

	public static void main(String[] args) {
		SubsequenceChecker.check("supercalifragilisticexpialidocious","pads");
		SubsequenceChecker.check("supercalifragilisticexpialidocious","padsx");
	}

}
