package com.example.appdev

import android.content.Context
import android.os.Handler
import android.os.Looper
import android.os.Message
import android.util.Log
import android.widget.TextView
import android.widget.Toast
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.util.concurrent.ConcurrentLinkedQueue

class MyHandler(looper: Looper) : Handler(looper) {
    // ... your custom methods and properties ...
    private lateinit var context: Context
    val floatDeque : ConcurrentLinkedQueue<Float> = ConcurrentLinkedQueue()

    override fun handleMessage(msg: Message) {
        when (msg.what) {
            MESSAGE_READ -> {
                val numBytes = msg.arg1
                val buffer = msg.obj as ByteArray
                val wrapped = ByteBuffer.wrap(buffer).order(ByteOrder.LITTLE_ENDIAN)
                val numFloats = numBytes / 4

                for (i in 0 until numFloats) {
                    floatDeque.add(wrapped.getFloat(i*4))
                }

                displayFloat(context, floatDeque)
                if(context !is GraphView) {
                    floatDeque.clear()
                }
//                val readMsg = String(buffer, 0, numBytes)
//                Log.d("Bluetooth","Received message: $readMsg")
//                // Update UI with the received message (using context)
//                updateUi(context, readMsg)
            }
            MESSAGE_WRITE -> {
                // Handle successful write operation (optional)
                Log.d("Handler", "Write operation successful")
            }
            MESSAGE_TOAST -> {
                val toastMsg = msg.data?.getString("toast")
                if (toastMsg != null) {
                    Toast.makeText(context, toastMsg, Toast.LENGTH_SHORT).show()
                }
            }
        }
    }

    fun updateUi(context: Context, message: String) {
        //Toast.makeText(context, message, Toast.LENGTH_LONG).show()
        if(context is btDemo) {
            context.runOnUiThread {
                // Update UI elements here
                context.findViewById<TextView>(R.id.message).text = "Console Output: $message"
            }
        }
    }

    fun displayFloat(context: Context, floatDeque: ConcurrentLinkedQueue<Float>) {
        if (context is btDemo) {
            context.runOnUiThread {
                // Update UI elements here
                val floatList = floatDeque.toList()
                context.findViewById<TextView>(R.id.message).text =
                    "Console Output: ${floatList.joinToString(", ")}"
            }
            floatDeque.clear()
        }
    }
        fun setContext(context: Context) {
            this.context = context
        }

        fun getContext(): Context {
            return context
        }


}