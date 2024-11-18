package com.example.appdev

import android.annotation.SuppressLint
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
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


private val MY_UUID : UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")
// ... (Add other message types here as needed.)

class MyBluetoothService(
    // handler that gets info from Bluetooth service
    private val handler: MyHandler,
    private val mBluetoothAdapter: BluetoothAdapter?,
    private val callback: BTStateListener?
) {

    lateinit var bluetoothThread : ConnectedThread

    //handle connected threads
    inner class ConnectedThread(private val mmSocket: BluetoothSocket) : Thread() {

        private val mmInStream: InputStream = mmSocket.inputStream
        private val mmOutStream: OutputStream = mmSocket.outputStream
        private val mmBuffer: ByteArray = ByteArray(1024) // mmBuffer store for the stream

        fun socket(): BluetoothSocket {
            return mmSocket
        }

        override fun run() {
            var numBytes: Int // bytes returned from read()
            Log.d(TAG, "ConnectedThread started")
            // Keep listening to the InputStream until an exception occurs.
            while (true) {
                // Read from the InputStream.
                numBytes = try {
                    mmInStream.read(mmBuffer)
                } catch (e: IOException) {
                    Log.d(TAG, "Input stream was disconnected", e)
                    callback?.updateBTState(false)
                    handler.post {
                        Toast.makeText(handler.getContext(), "Bluetooth Connection Lost", Toast.LENGTH_SHORT).show()
                    }
                    cancel()
                    break
                }
                Log.d(TAG, "Received bytes: ${String(mmBuffer, 0, numBytes)}")
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
                Log.d(TAG, "Write: ${String(bytes)}")
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
                    cancel()
                }


                // The connection attempt succeeded. Perform work associated with
                // the connection in a separate thread.
                bluetoothThread = ConnectedThread(socket)

                //make sure to start the connected thread
                bluetoothThread.start()


                if (bluetoothThread.socket().isConnected) {
                    Log.d("Bluetooth", "Bluetooth socket is connected")
                    callback?.updateBTState(true)
                    bluetoothThread.write("Connected to client successfully\n".toByteArray())
                } else {
                    Log.d("Bluetooth", "Bluetooth socket is not connected")
                }




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
