package com.m32s.ace.client;

import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Date;

import com.m32s.ace.client.Acemsgr.Login;

public class Client {

	private static final String host = "127.0.0.1";
	private InetAddress address;
	private int port = 8080;
	private Socket client;
	private DataOutputStream os;
	private DataInputStream is;
	
	private void connect() throws IOException {
		address = InetAddress.getByName(host);
		client = new Socket(address, port);

		os = new DataOutputStream(client.getOutputStream());
		is = new DataInputStream(client.getInputStream());
	}
	
	private Login buildLogin(int id, String name, String email)
			throws IOException {
		Login.Builder login = Login.newBuilder();

		login.setId(id);
		login.setName(name);
		if (email != null && email.length() > 0) {
			login.setEmail(email);
		}

		return login.build();
	}
	
	private void sendTestData() throws IOException {
		String s = "uranium";
		ByteBuffer buffer = ByteBuffer.allocate(4 + 4 + 4 + s.length());
		//buffer.order(ByteOrder.LITTLE_ENDIAN);
		buffer.putInt(2); // LOGIN
		buffer.putInt(1); // PID
		
		buffer.putInt(s.length());
		buffer.put(s.getBytes()); // Name
		
		os.write(buffer.array());
		os.flush();
		
		byte[] b = new byte[1024];
    	int read = is.read(b);
    	System.out.println("read: " + read );
        
        String response = byteArrayTohexString(b, 0, read);
        System.out.println("response: [" + response + "]");

		os.close();
		is.close();
	}
	
	private void sendTestProtoData() throws IOException {

		ByteArrayOutputStream bos = new ByteArrayOutputStream();
		
		int id = 1;
		String name = "uranium";
		String email = "sbrytskyy@gmail.com";

		Login login = buildLogin(id, name, email);
		login.writeTo(bos);

		// send Header
		int command = 2; // Login
		int pSize = bos.size();
		
		os.writeInt(command);
		os.flush();
		System.out.println("write command, " + new Date());

		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
		}
		
		os.writeInt(pSize);
		os.flush();
		System.out.println("write pSize, " + new Date());
		
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
		}

		bos.writeTo(os);
		os.flush();
		System.out.println("write body, " + new Date());

		byte[] b = new byte[1024];
    	int read = is.read(b);
    	System.out.println("read: " + read );
        
        String response = byteArrayTohexString(b, 0, read);
        System.out.println("response: [" + response + "]");
		
		os.close();
		is.close();
	}
	
	private String byteArrayTohexString(byte[] b, int begin, int count) {
		StringBuilder sb = new StringBuilder(count);
		for (int i = begin; i < begin + count; i++) {
			sb.append(String.format("%02x", b[i] & 0xff));
		}
		return sb.toString();
	}
	
	public Client() {
	}

	public static void main(String[] args) {
		Client m = new Client();
		try {
			m.connect();
			m.sendTestProtoData();
			//m.sendTestData();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

}
