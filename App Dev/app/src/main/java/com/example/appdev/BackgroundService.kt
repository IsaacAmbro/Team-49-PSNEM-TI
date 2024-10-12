package com.example.appdev

import android.app.Service
import android.bluetooth.BluetoothAdapter
import android.content.Intent
import android.os.Binder
import android.os.Handler
import android.os.IBinder
import android.os.Looper
import android.util.Log
import android.widget.Toast
import java.util.UUID

class BackgroundService : Service() {

    private var mBluetoothAdapter: BluetoothAdapter? = null
    private val MY_UUID : UUID = UUID.fromString("4af7db82-9136-45ea-af6a-62300fb0d8a4")
    lateinit var mBluetoothService: MyBluetoothService

    private val mHandler : Handler = Handler(
        Looper.getMainLooper()
    )

    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        Log.d("onStartCommand", "Started ")
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter()
        mBluetoothService = MyBluetoothService(mHandler, mBluetoothAdapter)
        mBluetoothService.startServer()
        return START_STICKY
    }


    override fun onDestroy() {
        super.onDestroy()
        mBluetoothService.stopServer()
    }

    override fun onBind(intent: Intent): IBinder {
        return MyBinder()
    }

    inner class MyBinder : Binder() {
        fun getService(): BackgroundService = this@BackgroundService
    }

}