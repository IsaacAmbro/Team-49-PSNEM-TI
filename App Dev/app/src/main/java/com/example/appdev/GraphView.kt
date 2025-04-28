package com.example.appdev
import android.app.Activity
import android.content.ComponentName
import android.content.Context
import android.content.Intent
import android.content.ServiceConnection
import android.graphics.Color
import android.net.Uri
import android.os.Bundle
import android.os.IBinder
import android.provider.DocumentsContract
import android.util.Log
import android.widget.Button
import android.widget.Toast
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.core.net.toUri
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat
import com.github.mikephil.charting.charts.LineChart
import com.github.mikephil.charting.components.XAxis
import com.github.mikephil.charting.data.Entry
import com.github.mikephil.charting.data.LineData
import com.github.mikephil.charting.data.LineDataSet
import androidx.lifecycle.lifecycleScope
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import java.io.File
import java.io.FileInputStream
import java.io.FileOutputStream
import java.util.concurrent.ConcurrentLinkedQueue
import kotlin.math.sin


// --- Constants ---

// Time delay (in arbitrary units, likely seconds or fraction of seconds) between consecutive x-values on the chart.
const val TIME_DELAY = 1f
// Maximum number of data points (entries) to display on the chart at once before clearing older points.
const val MAX_ENTRIES = 10000
// State flag indicating that data plotting is active.
const val STARTED : Int = 1
// State flag indicating that data plotting is stopped.
const val STOPPED : Int = 0
// Request code used for the Intent that allows the user to create/select a file for saving.
const val CREATE_FILE : Int = 1

/**
 * Activity responsible for displaying a real-time line graph of data received
 * from a background service. It allows starting/stopping
 * data plotting, clearing the graph and data, and saving the collected data to a CSV file.
 */

class GraphView : AppCompatActivity() {

    // ArrayLists to temporarily store X and Y values before writing them to the CSV file in batches.
    val xVal = ArrayList<Float>()
    val yVal = ArrayList<Float>()

    // Tracks the current state of the plotting process (STARTED or STOPPED).
    private var state = STOPPED

    // FileOutputStream for writing data to a temporary CSV file in the app's cache directory.
    lateinit var oStream: FileOutputStream
    // ConcurrentLinkedQueue to safely receive Float data points from the background service's thread.
    lateinit var BTdata: ConcurrentLinkedQueue<Float>

    // Reference to the bound BackgroundService instance.
    private lateinit var mService: BackgroundService

    /**
     * ServiceConnection implementation to manage the connection lifecycle with the BackgroundService.
     */
    private val connection = object : ServiceConnection {

        override fun onServiceConnected(className: ComponentName, service: IBinder) {
            val binder = service as BackgroundService.MyBinder
            mService = binder.getService()
            mService.setContext(this@GraphView)
            setFloatDeque()
            enableButtons()
        }

        override fun onServiceDisconnected(name: ComponentName) {

        }
    }

    /**
     * Retrieves the ConcurrentLinkedQueue for Float data from the BackgroundService's handler.
     */
    fun setFloatDeque() {
        BTdata = mService.mHandler.floatDeque
    }

    /**
     * Called when the activity is first created. Sets up the UI, initializes the chart,
     * binds to the background service, and configures button listeners.
     */
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(R.layout.activity_graph_view)
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main)) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom)
            insets
        }

        disableButtons()

        //start service to access bluetooth thread later
        Intent(this, BackgroundService::class.java).also { intent ->
            bindService(intent, connection, Context.BIND_AUTO_CREATE)
        }




        lateinit var chart: LineChart


        chart = findViewById(R.id.chart)
        var x = 0f
        var index = 0
        var y: Float

        val sineWave = Array(1000) { i ->
            val r = i * (2*Math.PI/100)
            4 * sin(r).toFloat()
        }


        val fileName = "temp.csv"
        val file = File(cacheDir,fileName)

        if(file.exists()) {
            file.delete()
        }

        oStream = FileOutputStream(file,true)
        createChart(chart)



        val startButton = findViewById<Button>(R.id.starter)
        val stopButton = findViewById<Button>(R.id.stopper)
        val saveButton = findViewById<Button>(R.id.saver)
        val clearButton = findViewById<Button>(R.id.clearer)

        saveButton.setOnClickListener{
            if (state == STOPPED) {
                createFile(file.toUri())
            }
        }

        clearButton.setOnClickListener{
            if(state == STOPPED) {
                val dialogBuilder = AlertDialog.Builder(this)
                dialogBuilder.setMessage("Confirm to clear all data")
                    .setCancelable(false)  // Prevent dismiss by clicking outside
                    .setPositiveButton("Confirm") { dialog, id ->

                        //reset arrays
                        xVal.clear()
                        yVal.clear()

                        //reset chart
                        x = 0f
                        chart.clear()
                        createChart(chart)

                        //reset csv
                        oStream.close()
                        oStream = FileOutputStream(file,false)
                        oStream.write("".toByteArray())
                        oStream.close()
                        oStream = FileOutputStream(file,true)
                    }
                    .setNegativeButton("Cancel") { dialog, id ->
                        dialog.dismiss() // Dismiss the dialog
                    }

                val alert = dialogBuilder.create()
                alert.show()



            }
        }


        startButton.setOnClickListener {
            if(state == STOPPED) {
                state = STARTED
                startButton.isEnabled = false
                saveButton.isEnabled = false
                clearButton.isEnabled = false
                mService.mHandler.start()


                lifecycleScope.launch {

                    val data = chart.data.getDataSetByIndex(0) as LineDataSet

                    //scrolling
                    launch {
                        while(state == STARTED){

                            delay(66)
                            updateView(chart, x)
                        }
                    }

                    //clearing if too big
                    launch {
                        while(state == STARTED){

                            Log.d("Entry Count: ", (xVal.size/1000).toString() + "k")

                            if(data.entryCount > MAX_ENTRIES) {

                                data.clear()
                                chart.moveViewToX(x)
                                chart.notifyDataSetChanged()

                            }

                            delay(1000)
                        }
                    }


                    //plotting
                    launch{
                        while (state == STARTED) {
                            for(i in 1..10) {
                            val poll = BTdata.poll()
                                if(poll != null) {
                                        y = poll
                                        addData(chart, x, y)
                                        xVal.add(x)
                                        yVal.add(y)
                                        x += TIME_DELAY
                                }
                            }

                            delay(1)
                        }
                    }

                    //csv writing
                    launch {
                            while(state == STARTED) {
                                if (xVal.size > 1000) {
                                    val subX = xVal.subList(0, 1000)
                                    val subY = yVal.subList(0, 1000)

                                    val rows = map(subX, subY)

                                    for (row in rows) {
                                        oStream.write((row + "\n").toByteArray())
                                    }

                                    Log.d("CSV", "wrote csv and xVal size:")
                                    subX.clear()
                                    subY.clear()
                                    Log.d("xVal size", xVal.size.toString())
                                }
                                delay(66)
                            }
                    }
                }
            }

        }


        //stop code
        stopButton.setOnClickListener{
            if(state == STARTED) {
                state = STOPPED
                mService.mHandler.stop()

                updateView(chart, x)

                //clear rest of xVal and yVal
                val rows = map(xVal,yVal)
                for (row in rows) {
                    oStream.write((row + "\n").toByteArray())
                }
                xVal.clear()
                yVal.clear()
                startButton.isEnabled = true
                saveButton.isEnabled = true
                clearButton.isEnabled = true
            }

        }







    }

    override fun onDestroy() {
        oStream.close()
        state = STOPPED
        super.onDestroy()
    }

    override fun onBackPressed() {
        // Show a confirmation dialog when the back button is pressed
        val builder = AlertDialog.Builder(this)
        builder.setMessage("Exiting will clear all data?")
            .setCancelable(false)
            .setPositiveButton("Exit") { dialog, id ->
                // Proceed with closing the activity
                super.onBackPressed()  // This actually performs the default back button action
            }
            .setNegativeButton("Cancel") { dialog, id ->
                dialog.dismiss()  // Dismiss the dialog if the user chooses "No"
            }

        val alert = builder.create()
        alert.show()
    }


    //check for null later because we won't have any data or maybe i force data
    fun addData(chart: LineChart, x: Float, y: Float) {
        val lineDataSet = chart.data.getDataSetByIndex(0) as LineDataSet
        lineDataSet.addEntry(Entry(x, y))
        chart.data.notifyDataChanged()

    }

    fun updateView(chart: LineChart, x: Float) {
        chart.data.notifyDataChanged()
        chart.notifyDataSetChanged()

        chart.moveViewToX(x)
        chart.setVisibleXRange(5f,5f)
        chart.invalidate()
    }


    fun map(x : List<Float>, y: List<Float>) : List<String> {
        val rows = x.indices.map { i -> "${x[i]},${y[i]}"}
        return rows
    }


    private fun createFile(pickerInitialUri: Uri) {
        val intent = Intent(Intent.ACTION_CREATE_DOCUMENT).apply {
            addCategory(Intent.CATEGORY_OPENABLE)
            type = "text/csv" // Set MIME type to CSV
            putExtra(Intent.EXTRA_TITLE, "untitled.csv") // Give the file a name

            // Optionally, specify a URI for the directory that should be opened in
            // the system file picker before your app creates the document.
            putExtra(DocumentsContract.EXTRA_INITIAL_URI, pickerInitialUri)
        }
        startActivityForResult(intent, CREATE_FILE)
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)

        if (requestCode == CREATE_FILE && resultCode == Activity.RESULT_OK) {
            data?.data?.let { uri ->
                // Once the file is created, you can write to it
                copyCsvFileToUri(uri)
            }
        }
    }

    private fun copyCsvFileToUri(destinationUri: Uri) {
        // Path to your existing CSV file (from cache or internal storage)
        val sourceFile = File(cacheDir, "temp.csv")  // Replace this with your actual file location

        try {
            contentResolver.openOutputStream(destinationUri)?.use { outputStream ->
                // Open input stream for the existing file
                FileInputStream(sourceFile).use { inputStream ->
                    // Copy the contents of the source CSV file to the selected destination
                    inputStream.copyTo(outputStream)

                }
            }
            Toast.makeText(this, "File saved successfully!", Toast.LENGTH_SHORT).show()
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    fun createChart(chart: LineChart) {
        val lineDataSet = LineDataSet(null, "Voltage")
        lineDataSet.color = Color.BLUE
        lineDataSet.lineWidth = 2f
        lineDataSet.setDrawCircles(false)
        lineDataSet.setDrawValues(false)

        chart.getDescription().setEnabled(false);

        val lineData = LineData(lineDataSet)


        chart.data = lineData

        val xAxis = chart.xAxis
        xAxis.position = XAxis.XAxisPosition.BOTTOM
        xAxis.setDrawGridLines(false)
        xAxis.setGranularity(2f) // One unit per X label



        chart.setTouchEnabled(false)
        chart.axisRight.isEnabled = false // Disable the right Y-axis
        chart.axisLeft.axisMinimum = -0.5f   // Set minimum Y value
        chart.axisLeft.axisMaximum = 2.5f // Set maximum Y value

        chart.invalidate() // Refreshes the chart with the new data


        chart.setVisibleXRange(5f,5f)

        chart.invalidate()
    }

    fun disableButtons() {
        val startButton = findViewById<Button>(R.id.starter)
        val stopButton = findViewById<Button>(R.id.stopper)
        val saveButton = findViewById<Button>(R.id.saver)
        val clearButton = findViewById<Button>(R.id.clearer)

        startButton.isEnabled = false
        stopButton.isEnabled = false
        saveButton.isEnabled = false
        clearButton.isEnabled = false
    }

    fun enableButtons() {
        val startButton = findViewById<Button>(R.id.starter)
        val stopButton = findViewById<Button>(R.id.stopper)
        val saveButton = findViewById<Button>(R.id.saver)
        val clearButton = findViewById<Button>(R.id.clearer)

        startButton.isEnabled = true
        stopButton.isEnabled = true
        saveButton.isEnabled = true
        clearButton.isEnabled = true
    }


}