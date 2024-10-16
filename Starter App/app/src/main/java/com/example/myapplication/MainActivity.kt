package com.example.myapplication

import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.view.View
import android.widget.TextView
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat

class MainActivity : AppCompatActivity() {

    // Initialize a counter variable
    private var counter = 0

    // Initialize a boolean variable to track if the counter is resetting
    private var isResetting = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(R.layout.activity_main)
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main)) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom)
            insets
        }


        // Find the button by its ID and set its click listener

        var button: View = findViewById(R.id.button)
        var reset: View = findViewById(R.id.reset)

        // Find the TextView by its ID and set its text
        var count: TextView = findViewById(R.id.count)

        // Set a click listener on the button
        button.setOnClickListener {
            // Increment the counter and update the TextView if reset is not being pressed
            Handler(Looper.getMainLooper()).postDelayed({
                if(!isResetting) {
                counter++
                count.setText("You clicked me $counter times")
                }
            }, 50)
        }

        reset.setOnClickListener {
            isResetting = true
            counter = 0
            count.setText("Click Below")

            // Introduce a short delay to prevent double button press
            Handler(Looper.getMainLooper()).postDelayed({
                isResetting = false
            }, 500)
        }


    }





}


