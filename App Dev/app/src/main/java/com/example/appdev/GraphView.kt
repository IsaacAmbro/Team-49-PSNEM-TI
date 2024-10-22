package com.example.appdev

import android.graphics.Color
import android.os.Bundle
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat
import com.github.mikephil.charting.charts.LineChart
import com.github.mikephil.charting.components.AxisBase
import com.github.mikephil.charting.components.Description
import com.github.mikephil.charting.components.XAxis
import com.github.mikephil.charting.data.Entry
import com.github.mikephil.charting.data.LineData
import com.github.mikephil.charting.data.LineDataSet
import com.github.mikephil.charting.formatter.IAxisValueFormatter
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale
import java.util.concurrent.TimeUnit
import kotlin.random.Random


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
        for (i in 1..10) {
            val randomY = Random.nextFloat() * 100 // Random Y values between 0 and 100
            entries.add(Entry(i.toFloat(), randomY))
        }

        val lineDataSet = LineDataSet(entries, "Random Data")
        lineDataSet.color = Color.BLUE
        lineDataSet.lineWidth = 2f
        lineDataSet.setDrawCircles(true)
        lineDataSet.circleRadius = 4f
        lineDataSet.setCircleColor(Color.RED)
        lineDataSet.setDrawValues(true)
        lineDataSet.valueTextColor = Color.BLACK
        lineDataSet.valueTextSize = 10f

        val lineData = LineData(lineDataSet)
        chart.data = lineData

        val xAxis = chart.xAxis
        xAxis.position = XAxis.XAxisPosition.BOTTOM
        xAxis.setDrawGridLines(false)
        xAxis.setGranularity(1f) // One unit per X label
        xAxis.labelCount = 10


        chart.axisRight.isEnabled = false // Disable the right Y-axis
        chart.axisLeft.axisMinimum = 0f   // Set minimum Y value
        chart.axisLeft.axisMaximum = 100f // Set maximum Y value

        chart.invalidate() // Refreshes the chart with the new data




    }
}