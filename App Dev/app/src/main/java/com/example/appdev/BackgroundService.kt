package com.example.appdev

import android.app.Service
import android.bluetooth.BluetoothAdapter
import android.content.Context
import android.content.Intent
import android.content.ServiceConnection
import android.os.Binder
import android.os.Handler
import android.os.IBinder
import android.os.Looper
import android.util.Log
import android.widget.Toast
import java.util.UUID


class BackgroundService : Service(){

    private var mBluetoothAdapter: BluetoothAdapter? = null
    lateinit var mBluetoothService: MyBluetoothService
    val mHandler = MyHandler(Looper.getMainLooper())


    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        Log.d("onStartCommand", "Started ")
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter()
        mBluetoothService = MyBluetoothService(mHandler, mBluetoothAdapter)
        return START_STICKY
    }


    override fun onDestroy() {
        super.onDestroy()
    }

    override fun onBind(intent: Intent): IBinder {
        return MyBinder()
    }

    inner class MyBinder : Binder() {
        fun getService(): BackgroundService = this@BackgroundService
    }

    fun setContext(context: Context) {
        mHandler.setContext(context)
    }

    fun isConnected() : Boolean {
        return mBluetoothService.getStatus()
    }


}

