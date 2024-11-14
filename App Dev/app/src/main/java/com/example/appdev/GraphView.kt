package com.example.appdev

import android.graphics.Color
import android.os.Bundle
import android.util.Log
import android.widget.Button
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat
import com.github.mikephil.charting.charts.LineChart
import com.github.mikephil.charting.components.XAxis
import com.github.mikephil.charting.data.Entry
import com.github.mikephil.charting.data.LineData
import com.github.mikephil.charting.data.LineDataSet
import kotlin.random.Random
import androidx.lifecycle.lifecycleScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.delay
import kotlinx.coroutines.isActive
import kotlinx.coroutines.launch
import kotlin.math.sin

const val TIME_DELAY = 0.005f
const val MAX_ENTRIES = 100000
const val STARTED : Int = 1
const val STOPPED : Int = 0


class GraphView : AppCompatActivity() {

    val xVal = ArrayList<Float>()
    val yVal = ArrayList<Float>()
    var state = STOPPED


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(R.layout.activity_graph_view)
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main)) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom)
            insets
        }

        val myDB = MyDatabaseHelper(this)

        //start service to access bluetooth thread later



        lateinit var chart: LineChart

        chart = findViewById(R.id.chart)


        val sineWave = Array(1000) { i ->
            val x = i * (2*Math.PI/100)
            25 * sin(x).toFloat() + 50
        }


        val startButton = findViewById<Button>(R.id.starter)

        startButton.setOnClickListener {
            if(state == STOPPED) {
                state = STARTED
                lifecycleScope.launch {
                    var x = 0f
                    var index = 0
                    var y = 0f
                    val data = chart.data.getDataSetByIndex(0) as LineDataSet
                    var jobDone = true

                    launch {
                        while(isActive){

                            delay(66)
                            updateView(chart, x)
    //                        Log.d("Data Set Count: ", chart.data.dataSetCount.toString())
    //                        Log.d("Entry Count: ", data.entryCount.toString())
                        }
                    }


                    launch {
                        while(isActive){
    //                        if(myDB.getRowCount() % 10000 == 0) {
    //                            Log.d("Entry Count: ", (myDB.getRowCount()/1000).toString() + "k")
    //                        }

    //                        if(chart.data.getDataSetByIndex(0).entryCount % 10000 == 0) {
    //
    //                            Log.d("Datasets : ", chart.data.dataSetCount.toString())
    //                        }

                            if(xVal.size % 10000 == 0) {
                                Log.d("Entry Count: ", (xVal.size/1000).toString() + "k")
                            }

                            if(chart.data.getDataSetByIndex(0).entryCount > MAX_ENTRIES) {
                                //data = createDataSet()
                                //chart.data.addDataSet(data)
                                //Log.d("Created ", chart.data.dataSetCount.toString())
                                //data.removeFirst()
                                //chart.notifyDataSetChanged()
                                data.clear()

                                chart.notifyDataSetChanged()

                            }

                            if(chart.data.dataSetCount > 3) {
                                val oldData = chart.data.getDataSetByIndex(0) as LineDataSet
                                oldData.clear()
                                chart.data.removeDataSet(oldData)
                                //oldData = null
                                chart.notifyDataSetChanged()
                                //Log.d("Deleted ", chart.data.dataSetCount.toString())
                            }

    //                        if(((x * 1000).toInt() % 100) == 0) Log.d("x: ", x.toString())


                            delay(1)
                        }
                    }

                    //consider adding to a new dataset every 1000 entries or so
                    launch{
                        while (isActive) {
                            //Log.d("Activity", "Running")

                            y = Random.nextFloat() * (80 - 20) + 20
                            for(i in 0 until 100){
                                //y = sineWave[index]
                                addData(chart, x, y)
                                xVal.add(x)
                                yVal.add(y)
                                x += TIME_DELAY
                                index++
                                if (index == 100) index = 0
                            }


    //
    //                        if (xVal.size > 100001 && jobDone) {
    //                            Log.d("log", "Got here")
    //                            val x100k = xVal.subList(0, 100000).toList()
    //                            val y100k = yVal.subList(0, 100000).toList()
    //
    //                            launch{
    //                                jobDone = false
    //                                    //Log.d("Entry", "x: " + x100k[i] + " y: " + y100k[i])
    //                                    myDB.insert(x100k, y100k)
    //
    //                            }
    //                            xVal.subList(0, 100000).clear()
    //                            yVal.subList(0, 100000).clear()
    //                            Log.d("log", "Ran to here")
    //                            jobDone = true
    //                        }
    //
    //                        if(!jobDone) {
    //                            Log.d("log", "Still running")
    //                        }
                            //Log.d("Entry Count After Rep: ", chart.data.getDataSetByIndex(0).entryCount.toString())
                            delay(45)
                        }
                    }
                }
            }
        }




        val lineDataSet = LineDataSet(null, "Dynamic Graph")
        lineDataSet.color = Color.BLUE
        lineDataSet.lineWidth = 2f
        lineDataSet.setDrawCircles(false)
        lineDataSet.setDrawValues(false)


        val lineData = LineData(lineDataSet)


        chart.data = lineData

        val xAxis = chart.xAxis
        xAxis.position = XAxis.XAxisPosition.BOTTOM
        xAxis.setDrawGridLines(false)
        xAxis.setGranularity(2f) // One unit per X label




        chart.axisRight.isEnabled = false // Disable the right Y-axis
        chart.axisLeft.axisMinimum = 0f   // Set minimum Y value
        chart.axisLeft.axisMaximum = 100f // Set maximum Y value

        chart.invalidate() // Refreshes the chart with the new data


        chart.setVisibleXRange(100f,100f)

        chart.invalidate()



    }

    override fun onDestroy() {
        super.onDestroy()
        state = STOPPED
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
        chart.setVisibleXRange(20f,20f)
        chart.moveViewToX(x)
        chart.invalidate()
    }

    fun createDataSet() : LineDataSet {
        var dataSet = LineDataSet(null, "Dynamic Graph")
        dataSet.color = Color.BLUE
        dataSet.lineWidth = 2f
        dataSet.setDrawCircles(false)
        dataSet.setDrawValues(false)
        return dataSet
    }


}