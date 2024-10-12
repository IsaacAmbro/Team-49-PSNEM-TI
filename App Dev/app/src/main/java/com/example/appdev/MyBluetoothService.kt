package com.example.appdev

import android.annotation.SuppressLint
import android.app.Service
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothServerSocket
import android.bluetooth.BluetoothSocket
import android.content.Context
import android.os.Bundle
import android.os.Handler
import android.util.Log
import android.widget.Toast
import java.io.IOException
import java.io.InputStream
import java.io.OutputStream
import java.util.UUID


private const val TAG = "MY_APP_DEBUG_TAG"

// Defines several constants used when transmitting messages between the
// service and the UI.
const val MESSAGE_READ: Int = 0
const val MESSAGE_WRITE: Int = 1
const val MESSAGE_TOAST: Int = 2


private val MY_UUID : UUID = UUID.fromString("4af7db82-9136-45ea-af6a-62300fb0d8a4")
// ... (Add other message types here as needed.)

class MyBluetoothService(
    // handler that gets info from Bluetooth service
    private val handler: Handler,
    private val mBluetoothAdapter: BluetoothAdapter?
) {

    lateinit var bluetoothThread : ConnectedThread

    //handle connected threads
    inner class ConnectedThread(private val mmSocket: BluetoothSocket) : Thread() {

        private val mmInStream: InputStream = mmSocket.inputStream
        private val mmOutStream: OutputStream = mmSocket.outputStream
        private val mmBuffer: ByteArray = ByteArray(1024) // mmBuffer store for the stream

        override fun run() {
            var numBytes: Int // bytes returned from read()

            // Keep listening to the InputStream until an exception occurs.
            while (true) {
                // Read from the InputStream.
                numBytes = try {
                    mmInStream.read(mmBuffer)
                } catch (e: IOException) {
                    Log.d(TAG, "Input stream was disconnected", e)
                    break
                }

                // Send the obtained bytes to the UI activity.
                val readMsg = handler.obtainMessage(
                    MESSAGE_READ, numBytes, -1,
                    mmBuffer)
                readMsg.sendToTarget()
            }
        }

        // Call this from the main activity to send data to the remote device.
        fun write(bytes: ByteArray) {
            try {
                mmOutStream.write(bytes)
            } catch (e: IOException) {
                Log.e(TAG, "Error occurred when sending data", e)

                // Send a failure message back to the activity.
                val writeErrorMsg = handler.obtainMessage(MESSAGE_TOAST)
                val bundle = Bundle().apply {
                    putString("toast", "Couldn't send data to the other device")
                }
                writeErrorMsg.data = bundle
                handler.sendMessage(writeErrorMsg)
                return
            }

            // Share the sent message with the UI activity.
            val writtenMsg = handler.obtainMessage(
                MESSAGE_WRITE, -1, -1, mmBuffer)
            writtenMsg.sendToTarget()
        }

        // Call this method from the main activity to shut down the connection.
        fun cancel() {
            try {
                mmSocket.close()
            } catch (e: IOException) {
                Log.e(TAG, "Could not close the connect socket", e)
            }
        }
    }

    private fun manageMyConnectedSocket(socket: BluetoothSocket) {
        val connectedThread = ConnectedThread(socket)
        connectedThread.start() // Start the thread to handle communication
    }

    @SuppressLint("MissingPermission")
    inner class AcceptThread : Thread() {

        private val mmServerSocket: BluetoothServerSocket? by lazy(LazyThreadSafetyMode.NONE) {
            mBluetoothAdapter?.listenUsingInsecureRfcommWithServiceRecord(mBluetoothAdapter!!.name, MY_UUID)
        }

        override fun run() {
            // Keep listening until exception occurs or a socket is returned.
            var shouldLoop = true
            while (shouldLoop) {
                val socket: BluetoothSocket? = try {
                    Log.d("Bluetooth Server", "Server Started ")
                     mmServerSocket?.accept()
                } catch (e: IOException) {
                    Log.e("Bluetooth Error", "Socket's accept() method failed", e)
                    shouldLoop = false
                    null
                }
                socket?.also {
                    mmServerSocket?.close()
                    bluetoothThread = ConnectedThread(it)
                    Log.d("Bluetooth Server", "Server Socket Closed ")
                    shouldLoop = false
                }
            }
        }

        // Closes the connect socket and causes the thread to finish.
        fun cancel() {
            try {
                mmServerSocket?.close()
            } catch (e: IOException) {
                Log.e("Close error", "Could not close the connect socket", e)
            }
        }
    }

    private val acceptThread: AcceptThread? by lazy {
        AcceptThread()
    }

    fun startServer() {
        acceptThread?.start()
    }

    fun stopServer() {
        acceptThread?.cancel()
    }


    //connection code
    @SuppressLint("MissingPermission")
    inner class ConnectThread(device: BluetoothDevice) : Thread() {

        private val mmSocket: BluetoothSocket? by lazy(LazyThreadSafetyMode.NONE) {
            device.createRfcommSocketToServiceRecord(MY_UUID)
        }

        public override fun run() {
            // Cancel discovery because it otherwise slows down the connection.
            mBluetoothAdapter?.cancelDiscovery()

            mmSocket?.let { socket ->
                // Connect to the remote device through the socket. This call blocks
                // until it succeeds or throws an exception.
                try {
                    socket.connect()
                    Log.d("Bluetooth", "Connecting...")
                } catch (e: IOException) {
                    Log.e("Bluetooth Error", "Error connecting to socket", e)
                }


                // The connection attempt succeeded. Perform work associated with
                // the connection in a separate thread.
                bluetoothThread = ConnectedThread(socket)
            }
        }

        // Closes the client socket and causes the thread to finish.
        fun cancel() {
            try {
                mmSocket?.close()
            } catch (e: IOException) {
                Log.e("Bluetooth Error", "Could not close the client socket", e)
            }
        }
    }

    @SuppressLint("MissingPermission")
    fun connect(context: Context, device: BluetoothDevice) {
        val connectThread = ConnectThread(device)
        connectThread.start()
        Toast.makeText(context, "Connected to ${device.name}", Toast.LENGTH_LONG).show()
    }

}
