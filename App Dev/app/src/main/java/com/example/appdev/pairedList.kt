package com.example.appdev

import android.annotation.SuppressLint
import android.app.Activity
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.content.ComponentName
import android.content.Context
import android.content.Intent
import android.content.ServiceConnection
import android.os.Bundle
import android.os.IBinder

import android.widget.AdapterView
import android.widget.ArrayAdapter
import android.widget.Button
import android.widget.ListView
import android.widget.Toast
import androidx.activity.enableEdgeToEdge
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat


class pairedList : AppCompatActivity() {

    private var mBluetoothAdapter: BluetoothAdapter? = null
    lateinit var mPairedDevices: Set<BluetoothDevice>
    private lateinit var mService: BackgroundService

    private val connection = object : ServiceConnection {

        override fun onServiceConnected(className: ComponentName, service: IBinder) {
            val binder = service as BackgroundService.MyBinder
            mService = binder.getService()
            mService.setContext(this@pairedList)
        }

        override fun onServiceDisconnected(name: ComponentName) {

        }
    }


    val registerForResult = registerForActivityResult(
        ActivityResultContracts.StartActivityForResult()
    ) { result ->
        if (result.resultCode != Activity.RESULT_OK) {
            Toast.makeText(this, "Bluetooth is disabled. Please enable it.", Toast.LENGTH_LONG).show()
        } else {
            Toast.makeText(this, "Bluetooth is enabled.", Toast.LENGTH_LONG).show()
        }
    }


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(R.layout.activity_paired_list)
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main)) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom)
            insets
        }

        Intent(this, BackgroundService::class.java).also { intent ->
            bindService(intent, connection, Context.BIND_AUTO_CREATE)
        }





        if (mBluetoothAdapter?.isEnabled == false) {
            val enableBtIntent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
            registerForResult.launch(enableBtIntent)
        }

        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter()


        val refresh = findViewById<Button>(R.id.refresh)
        refresh.setOnClickListener { pairedDeviceList() }


    }


    // Get the already paired devices
    @SuppressLint("MissingPermission")
    fun pairedDeviceList(){
        mPairedDevices = mBluetoothAdapter!!.bondedDevices
        val list : ArrayList<BluetoothDevice> = ArrayList()
        val names : ArrayList<String> = ArrayList()
        if(!mPairedDevices.isEmpty()){
            for(device: BluetoothDevice in mPairedDevices){
                list.add(device)
                names.add(device.name ?: "Unknown Device")
            }
        }


        else{
            Toast.makeText(this, "No Paired Devices", Toast.LENGTH_LONG).show()
        }


        //Put the device names in a listview
        val adapater = ArrayAdapter(this, android.R.layout.simple_list_item_1, names)

        val pairedList = findViewById<ListView>(R.id.pairedList)
        pairedList.adapter = adapater
        pairedList.onItemClickListener = AdapterView.OnItemClickListener{ _,_,position, _ ->
            val device : BluetoothDevice = list[position]


            if(device.name == "ESP 32 Bluetooth Server") {

                    mService.mBluetoothService.connect(this, device)

            }
            //pairedList.isEnabled = false
        }
    }


    override fun onDestroy() {
        unbindService(connection)
        super.onDestroy()

    }

}

