// Daniel Hanna
// Computer Networking 
// Project 2: Building a Multi-threaded Server

// Importing appripiate libraries 
import java.io.* ;
import java.net.* ;
import java.util.* ;
import java.nio.channels.*; 	// Channels library is needed for ServerSocketChannel


public final class webServer
{
	public static void main(String argv[]) throws Exception
	{  

		Selector selector = Selector.open();  
		int ports[] = new int[] { 8888, 5555 };		// We will be listening to ports 8888 and 5555

		for (int port : ports) {
			ServerSocketChannel ssc = ServerSocketChannel.open();
			ssc.configureBlocking(false);
			ssc.socket().bind(new InetSocketAddress(port));		// We need to bind the ports to a socket
			ssc.register(selector, SelectionKey.OP_ACCEPT);  
			ssc.accept(); 										// Accept the connection
		} 

		while (true) {				// Enter infinite loop for listening for requests
			selector.select();
			Iterator<SelectionKey> selectedKeys = selector.selectedKeys().iterator();
			while (selectedKeys.hasNext()) {
				SelectionKey selectedKey = selectedKeys.next();
				selectedKeys.remove(); 

				if (selectedKey.isAcceptable()) {
					SocketChannel socketChannel = ((ServerSocketChannel) selectedKey.channel()).accept();
					switch (socketChannel.socket().getLocalPort()) {
						case 8888:				// If port is 8888, use HttpRequest to process the request
							Socket connectionSocket8888 = socketChannel.socket(); 
							HttpRequest request8888 = new HttpRequest(connectionSocket8888); 
							Thread thread8888 = new Thread(request8888); 		// Initate a new thread and begin it
							thread8888.start(); 
							break; 
						case 5555:				// If port is 5555, use MovedRequest instead
							Socket connectionSocket5555 = socketChannel.socket(); 
							MovedRequest request5555 = new MovedRequest(connectionSocket5555); 
							Thread thread5555 = new Thread(request5555); 
							thread5555.start(); 
							break;
					}
				}
			}
		}
	}
}

final class HttpRequest implements Runnable
{
	final static String CRLF = "\r\n";
	Socket socket;

	// Constructor
	public HttpRequest(Socket socket) throws Exception
	{
		this.socket = socket;
	}

	// Implement the run() method of the Runnable interface.
	public void run()
	{
		try {
			processRequest(); 
		}
		catch (Exception e) {
			System.out.println(e); 
		}
	}

	private void processRequest() throws Exception
	{
		// Get a reference to the socket's input and output streams.
		InputStream is = socket.getInputStream();
		DataOutputStream os = new DataOutputStream(socket.getOutputStream());

		// Set up input stream filters.
		BufferedReader br = new BufferedReader(new InputStreamReader(is));
		
		// Get the request line of the HTTP request message.
		String requestLine = br.readLine();

		// Extract the filename from the request line.
		StringTokenizer tokens = new StringTokenizer(requestLine);
		tokens.nextToken(); // skip over the method, which should be "GET"
		String fileName = tokens.nextToken(); 

		// Prepend a "." so that file request is within the current directory.
		fileName = "." + fileName; 

		// Open the requested file.
		FileInputStream fis = null;
		boolean fileExists = true;
		try {
			fis = new FileInputStream(fileName);
		} 
		catch (FileNotFoundException e) {
			fileExists = false;
		} 

		// Construct the response message.
		String statusLine = null;
		String contentTypeLine = null;
		String entityBody = null;
		if (fileExists) {
			statusLine = "HTTP/1.0 200 OK" + CRLF; 
			contentTypeLine = "Content-type: " + contentType(fileName) + CRLF; 
		}
		else {
			statusLine =  "HTTP/1.0 404 Not Found" + CRLF; 
			contentTypeLine = "Content-type: " + "text/html" + CRLF; 
			entityBody = "<HTML>" +
			"<HEAD><TITLE>Not Found</TITLE></HEAD>" +
			"<BODY>Not Found</BODY></HTML>";
		}

		// Send the status line.
		os.writeBytes(statusLine); 

		// Send the content type line. 
		os.writeBytes(contentTypeLine); 

		// Send a blank line to indicate the end of the header lines.
		os.writeBytes(CRLF); 

		// Send the entity body.
		if (fileExists) {
			sendBytes(fis, os); 
			fis.close(); 
		}
		else {
			os.writeBytes(entityBody); 
		}

		// Display the request line.
		System.out.println();
		System.out.println(requestLine);

		// Get and display the header lines.
		String headerLine = null;
		while ((headerLine = br.readLine()).length() != 0) {
			System.out.println(headerLine);
		}		

		os.close();
		br.close();
		socket.close();
	}

	private static void sendBytes(FileInputStream fis, OutputStream os)
	throws Exception
	{
		// Construct a 1K buffer to hold bytes on their way to the socket.
		byte[] buffer = new byte[1024];
		int bytes = 0;

		// Copy requested file into the socket's output stream.
		while((bytes = fis.read(buffer)) != -1 ) {
			os.write(buffer, 0, bytes);
		}
	}

	private static String contentType(String fileName)
	{
		if (fileName.endsWith(".htm") || fileName.endsWith(".html")) {		// Use file ending to find the content type
			return "text/html";
		}
		if (fileName.endsWith(".jpg") || fileName.endsWith(".jpeg")) {
			return "image/jpeg";
		}
		if (fileName.endsWith(".gif")) {
			return "image/gif";
		}
		if (fileName.endsWith(".txt")) {
			return "text/plain";
		}
		if (fileName.endsWith(".pdf")) {
			return "application/pdf";
		}
		return "application/octet-stream";
	}
}

final class MovedRequest implements Runnable {			// MovedRequest is almost identical to HttpRequest			
	final static String CRLF = "\r\n";
	Socket socket;

	// Constructor
	public MovedRequest(Socket socket) throws Exception
	{
		this.socket = socket;
	}

	// Implement the run() method of the Runnable interface.
	public void run()
	{
		try {
			processRequest(); 
		}
		// We know that processing the request will result in a broken pipe. That is fine because the intention will be to redirect anyways
		catch (Exception e) {		
			System.out.println("HTTP/1.1 301 Moved Permanently"); 
		}
	}

	private void processRequest() throws Exception
	{
		// Get a reference to the socket's input and output streams.
		InputStream is = socket.getInputStream();
		DataOutputStream os = new DataOutputStream(socket.getOutputStream());

		// Set up input stream filters.
		BufferedReader br = new BufferedReader(new InputStreamReader(is));
		
		// Get the request line of the HTTP request message.
		String requestLine = br.readLine();

		// Extract the filename from the request line.
		StringTokenizer tokens = new StringTokenizer(requestLine);
		tokens.nextToken(); // skip over the method, which should be "GET"

		// The "file" is going to be the URL we want to redirect to
		String fileName = "http://www.google.com";

		// Open the requested file.
		FileInputStream fis = null;
		boolean fileExists = true;

		// Construct the response message.
		String statusLine = null;
		String contentTypeLine = null;
		String entityBody = null;
		statusLine = "HTTP/1.1 301 Moved Permanently" + CRLF; 		// The status will be a 301 Moved Permanently
		contentTypeLine = "Location: " + fileName + CRLF; 			// The new location will be the URL we want
		entityBody = "<HTML>" +
		"<HEAD><TITLE>Moved Permanently</TITLE></HEAD>" +
		"<BODY>Moved Permanently</BODY></HTML>";

		// Send the status line.
		os.writeBytes(statusLine); 

		// Send the content type line. 
		os.writeBytes(contentTypeLine); 

		// Send a blank line to indicate the end of the header lines.
		os.writeBytes(CRLF); 
		os.writeBytes(entityBody); 

		// Display the request line.
		System.out.println();
		System.out.println(statusLine); 
		System.out.println(requestLine);

		// Get and display the header lines.
		String headerLine = null;
		while ((headerLine = br.readLine()).length() != 0) {
			System.out.println(headerLine);
		}		

		os.close();
		br.close();
		socket.close();
	}

	private static void sendBytes(FileInputStream fis, OutputStream os)
	throws Exception
	{
		// Construct a 1K buffer to hold bytes on their way to the socket.
		byte[] buffer = new byte[1024];
		int bytes = 0;

		// Copy requested file into the socket's output stream.
		while((bytes = fis.read(buffer)) != -1 ) {
			os.write(buffer, 0, bytes);
		}
	}

	private static String contentType(String fileName)
	{
		if (fileName.endsWith(".htm") || fileName.endsWith(".html")) {
			return "text/html";
		}
		if (fileName.endsWith(".jpg") || fileName.endsWith(".jpeg")) {
			return "image/jpeg";
		}
		if (fileName.endsWith(".gif")) {
			return "image/gif";
		}
		if (fileName.endsWith(".txt")) {
			return "text/plain";
		}
		if (fileName.endsWith(".pdf")) {
			return "application/pdf";
		}
		return "application/octet-stream";
	}
}