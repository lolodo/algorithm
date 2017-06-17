import java.io.*;
// import java.io.BufferedReader;

public class StreamIO {
    // public static void streamInputChar() {
    //     char c;
    //     BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
    //     System.out.println("Press q to quit!");

    //     do {
    //         c = (char)br.read();
    //         System.out.println(c);
    //     } while(c != 'q');
    // }

    // public static void streamInputString() {
    //     BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
    //     String str;

    //     do {
    //         str = br.readLine();
    //         System.out.println(str);
    //     } while(!str.equals("end"));
    // }
    public static void main(String[] args) throws IOException
    {
        // streamInputString();
        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
        String str;

        do {
            str = br.readLine();
            System.out.println(str);
        } while(!str.equals("end"));
    }
}