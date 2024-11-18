package com.example.appdev

import android.content.ComponentName
import android.content.Context
import android.content.Intent
import android.content.ServiceConnection
import android.os.Bundle
import android.os.IBinder
import android.widget.Button

import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat

class btDemo : AppCompatActivity() {
    private lateinit var mService: BackgroundService

    private val connection = object : ServiceConnection {

        override fun onServiceConnected(className: ComponentName, service: IBinder) {
            val binder = service as BackgroundService.MyBinder
            mService = binder.getService()
            mService.setContext(this@btDemo)
        }

        override fun onServiceDisconnected(name: ComponentName) {

        }
    }



    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(R.layout.activity_bt_demo)
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main)) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom)
            insets
        }

        Intent(this, BackgroundService::class.java).also { intent ->
            bindService(intent, connection, Context.BIND_AUTO_CREATE)
        }


        val sender = findViewById<Button>(R.id.send)
        sender.setOnClickListener {
            if (mService.isConnected()) {
                val bluetoothService = mService.mBluetoothService
                bluetoothService.bluetoothThread?.let {
                    it.write("switch 1\n".toByteArray())
                }
            }
        }

        val sender2 = findViewById<Button>(R.id.send2)
        sender2.setOnClickListener {
            if (mService.isConnected()) {
                val bluetoothService = mService.mBluetoothService
                bluetoothService.bluetoothThread?.let {
                    it.write("switch 2\n".toByteArray())
                }
            }
        }

        val sender3 = findViewById<Button>(R.id.send3)
        sender3.setOnClickListener {
            if (mService.isConnected()) {
                val bluetoothService = mService.mBluetoothService
                bluetoothService.bluetoothThread?.let {
                    it.write("switch 3\n".toByteArray())
                }
            }
        }
    }

    override fun onDestroy() {
        unbindService(connection)
        super.onDestroy()
    }
}