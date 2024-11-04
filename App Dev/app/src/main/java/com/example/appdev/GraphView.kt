package com.example.appdev

import android.graphics.Color
import android.os.Bundle
import android.util.Log
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
import kotlinx.coroutines.delay
import kotlinx.coroutines.isActive
import kotlinx.coroutines.launch
import kotlin.math.sin


class GraphView : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(R.layout.activity_graph_view)
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main)) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom)
            insets
        }


        //start service to access bluetooth thread later

        lateinit var chart: LineChart

        chart = findViewById(R.id.chart)

        val entries = ArrayList<Entry>()

        val sineWave = Array(100) { i ->
            val x = i * (2*Math.PI/100)
            25 * sin(x).toFloat() + 50
        }

        // Generate random data for the chart
        for (i in 1..10) {
            val randomY = Random.nextFloat() * 100 // Random Y values between 0 and 100
            entries.add(Entry(i.toFloat(), randomY))
        }






        val lineDataSet = LineDataSet(entries, "Random Data")
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


        //xAxis.labelCount = 10


        chart.axisRight.isEnabled = false // Disable the right Y-axis
        chart.axisLeft.axisMinimum = 0f   // Set minimum Y value
        chart.axisLeft.axisMaximum = 100f // Set maximum Y value

        chart.invalidate() // Refreshes the chart with the new data


        //val chartData = chart.data
        //chartData.addEntry(Entry(11f, 50f), 0)


        lineDataSet.addEntry(Entry(11f, 50f))
        chart.data.notifyDataChanged()
        chart.notifyDataSetChanged()

        chart.setVisibleXRange(5f,5f)
        chart.moveViewToX(11f)


        chart.invalidate()

        lifecycleScope.launch {
            var x = 12f
            var index = 0;
            var y = sineWave[index % 100]

           while(isActive) {

               addData(chart, x, y)

               x = x + 0.1f
               index++
               y = sineWave[index % 100]


               addData(chart, x, y)

               x = x + 0.1f
               index++
               y = sineWave[index % 100]

               addData(chart, x, y)

               x = x + 0.1f
               index++
               y = sineWave[index % 100]


               delay(66)
               if( y > 98) {
                   y = 0f;
               }
            }
        }

    }


    //check for null later because we won't have any data or maybe i force data
    fun addData(chart: LineChart, x: Float, y: Float) {
        val lineDataSet = chart.data.getDataSetByIndex(0) as LineDataSet
        lineDataSet.addEntry(Entry(x, y))
        chart.data.notifyDataChanged()
        chart.notifyDataSetChanged()
        chart.setVisibleXRange(20f,20f)
        chart.moveViewToX(x)
        chart.invalidate()
    }
}