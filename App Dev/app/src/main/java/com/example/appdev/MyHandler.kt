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
    var state : Boolean = false

    override fun handleMessage(msg: Message) {
        when (msg.what) {
            MESSAGE_READ -> {
                val numBytes = msg.arg1
                val buffer = msg.obj as ByteArray
                val wrapped = ByteBuffer.wrap(buffer).order(ByteOrder.LITTLE_ENDIAN)
                val numFloats = numBytes / 4

                if(state) {
                    for (i in 0 until numFloats) {
                        floatDeque.add(wrapped.getFloat(i * 4))
                    }
                }

                if(context is btDemo) {
                    val displayDeque : ConcurrentLinkedQueue<Float> = ConcurrentLinkedQueue()
                    for(i in 0 until numFloats) {
                        displayDeque.add(wrapped.getFloat(i*4))
                    }
                    displayFloat(context, displayDeque)
                    displayDeque.clear()
                }

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

        fun start() {
            state = true
        }

        fun stop() {
            state = false
        }
}