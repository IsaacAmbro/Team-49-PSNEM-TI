package com.example.appdev

import android.annotation.SuppressLint
import android.app.Activity
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.content.Intent
import android.os.Bundle
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
    private val REQUEST_ENABLE_BLUETOOTH = 1
    lateinit var mPairedDevices: Set<BluetoothDevice>



    val registerForResult = registerForActivityResult(
        ActivityResultContracts.StartActivityForResult()
    ) { result ->
        if (result.resultCode != Activity.RESULT_OK) {
            Toast.makeText(this, "Bluetooth is disabled. Please enable it.", Toast.LENGTH_LONG).show()
        } else {
            Toast.makeText(this, "Bluetooth is enabled.", Toast.LENGTH_LONG).show()
        }
    }

    companion object {
        val EXTRA_ADDRESS: String = "Device_address"
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


        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter()

        if (mBluetoothAdapter?.isEnabled == false) {
            val enableBtIntent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
            registerForResult.launch(enableBtIntent)
        }

        val refresh = findViewById<Button>(R.id.refresh)
        refresh.setOnClickListener { pairedDeviceList() }


    }


    // Get the already paired devices
    @SuppressLint("MissingPermission")
    private fun pairedDeviceList(){
        mPairedDevices = mBluetoothAdapter!!.bondedDevices
        val list : ArrayList<BluetoothDevice> = ArrayList()

        if(!mPairedDevices.isEmpty()){
            for(device: BluetoothDevice in mPairedDevices){
                list.add(device)
            }
        }
        else{
            Toast.makeText(this, "No Paired Devices", Toast.LENGTH_LONG).show()
        }

        val adapater = ArrayAdapter(this, android.R.layout.simple_list_item_1, list)

        val pairedList = findViewById<ListView>(R.id.pairedList)
        pairedList.adapter = adapater
        pairedList.onItemClickListener = AdapterView.OnItemClickListener{ _,_,position, _ ->
            val device : BluetoothDevice = list[position]
            val address: String = device.address

        }
    }
}