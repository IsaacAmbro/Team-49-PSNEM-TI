package com.example.appdev


import android.Manifest
import android.annotation.SuppressLint
import android.app.Activity
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothServerSocket
import android.bluetooth.BluetoothSocket
import android.content.ComponentName
import android.content.Context
import android.content.Intent
import android.content.ServiceConnection
import android.os.Build
import android.os.Bundle
import android.os.Handler
import android.os.IBinder
import android.util.Log
import android.widget.Button
import android.widget.Toast
import androidx.activity.enableEdgeToEdge
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat
import java.io.IOException
import java.util.UUID

class MainActivity : AppCompatActivity() {

    private var mBluetoothAdapter: BluetoothAdapter? = null
    private val REQUEST_ENABLE_BLUETOOTH = 1
    private var serverSocket: BluetoothServerSocket? = null
    private val MY_UUID : UUID = UUID.fromString("4af7db82-9136-45ea-af6a-62300fb0d8a4")
    private lateinit var mHandler : Handler
    private lateinit var mService: BackgroundService

    private val connection = object : ServiceConnection {

        override fun onServiceConnected(className: ComponentName, service: IBinder) {
            val binder = service as BackgroundService.MyBinder
            mService = binder.getService()
            mService.setContext(this@MainActivity)
        }

        override fun onServiceDisconnected(name: ComponentName) {

        }
    }




    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(R.layout.activity_main)
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main)) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom)
            insets
        }

        requestBluetooth()

        val switchList = findViewById<Button>(R.id.switchList)
        switchList.setOnClickListener {
            Log.d("Button", "Pushed ")
            Intent(this, pairedList::class.java).also {
                startActivity(it)
            }
        }


        // Check if bluetooth enabled
        if (mBluetoothAdapter?.isEnabled == false) {
            val enableBtIntent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
            registerForResult.launch(enableBtIntent)
        }

        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter()
        startService(Intent(this, BackgroundService::class.java))


        Intent(this, BackgroundService::class.java).also { intent ->
            bindService(intent, connection, Context.BIND_AUTO_CREATE)
        }




    }
//        val acceptThread: AcceptThread? by lazy {
//            AcceptThread()
//        }
//
//        acceptThread?.start()
//    }
//
//    @SuppressLint("MissingPermission")
//    private inner class AcceptThread : Thread() {
//
//        private val mmServerSocket: BluetoothServerSocket? by lazy(LazyThreadSafetyMode.NONE) {
//            mBluetoothAdapter?.listenUsingInsecureRfcommWithServiceRecord(mBluetoothAdapter!!.name, MY_UUID)
//        }
//
//        override fun run() {
//            // Keep listening until exception occurs or a socket is returned.
//            var shouldLoop = true
//            while (shouldLoop) {
//                val socket: BluetoothSocket? = try {
//                    Log.d("Bluetooth Server", "Server Started ")
//                    mmServerSocket?.accept()
//                } catch (e: IOException) {
//                    Log.e("Bluetooth Error", "Socket's accept() method failed", e)
//                    shouldLoop = false
//                    null
//                }
//                socket?.also {
//                    mmServerSocket?.close()
//                    Log.d("Bluetooth Server", "Server Closed ")
//                    shouldLoop = false
//                }
//            }
//        }
//
//        // Closes the connect socket and causes the thread to finish.
//        fun cancel() {
//            try {
//                mmServerSocket?.close()
//            } catch (e: IOException) {
//                Log.e("Close error", "Could not close the connect socket", e)
//            }
//        }
//    }


    val registerForResult = registerForActivityResult(
        ActivityResultContracts.StartActivityForResult()
    ) { result ->
        if (result.resultCode != Activity.RESULT_OK) {
            Toast.makeText(this, "Bluetooth is disabled. Please enable it.", Toast.LENGTH_LONG)
                .show()
        } else {
            Toast.makeText(this, "Bluetooth is enabled.", Toast.LENGTH_LONG).show()
        }
    }

    //requesting bluetooth permissions
    private val requestEnableBluetooth =
        registerForActivityResult(ActivityResultContracts.StartActivityForResult()) { result ->
            if (result.resultCode == RESULT_OK) {
                // granted
            } else {
                // denied
            }
        }

    private val requestMultiplePermissions =
        registerForActivityResult(ActivityResultContracts.RequestMultiplePermissions()) { permissions ->
            permissions.entries.forEach {
                Log.d("MyTag", "${it.key} = ${it.value}")
            }
        }

    fun requestBluetooth() {
        // check android 12+
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            requestMultiplePermissions.launch(
                arrayOf(
                    Manifest.permission.BLUETOOTH_SCAN,
                    Manifest.permission.BLUETOOTH_CONNECT,
                )
            )
        } else {
            val enableBtIntent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
            requestEnableBluetooth.launch(enableBtIntent)
        }
    }
}
