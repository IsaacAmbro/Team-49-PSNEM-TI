package com.example.appdev

import android.content.ContentValues
import android.content.Context
import android.database.sqlite.SQLiteDatabase
import android.database.sqlite.SQLiteOpenHelper
import android.util.Log
import com.example.appdev.DatabaseSchema.DATABASE_NAME
import com.example.appdev.DatabaseSchema.DATABASE_VERSION


//Database scheme
object DatabaseSchema {
    const val DATABASE_NAME = "graph.db"
    const val DATABASE_VERSION = 3
    const val TABLE_NAME = "CSV"
    const val COLUMN_ID = "ID"
    const val COLUMN_X = "X_Val"
    const val COLUMN_Y = "Y_val"

}

class MyDatabaseHelper(context: Context) : SQLiteOpenHelper(context, DATABASE_NAME, null, DATABASE_VERSION) {


    private val sqlQuery =
        "CREATE TABLE ${DatabaseSchema.TABLE_NAME} (" +
            "${DatabaseSchema.COLUMN_ID} INTEGER PRIMARY KEY AUTOINCREMENT, " +
            "${DatabaseSchema.COLUMN_X} REAL, " +
            "${DatabaseSchema.COLUMN_Y} REAL)"



    public override fun onCreate(p0: SQLiteDatabase?) {
        p0?.execSQL(sqlQuery)

    }

    public override fun onUpgrade(p0: SQLiteDatabase?, p1: Int, p2: Int) {
        p0?.execSQL("DROP TABLE IF EXISTS ${DatabaseSchema.TABLE_NAME}")
        onCreate(p0)
    }


    // Method to insert a new coordinate
    fun insert(x: Float, y: Float){
        val db = writableDatabase
        val values = ContentValues().apply {
            put(DatabaseSchema.COLUMN_X, x)
            put(DatabaseSchema.COLUMN_Y, y)
        }
        val result = db.insert(DatabaseSchema.TABLE_NAME, null, values)
        db.close()
        if(result == -1L){
            Log.d("Database", "Failed to insert coordinate")
        }
    }

    fun insert(xList: List<Float>, yList: List<Float>) {
        if (xList.size != yList.size) {
            Log.d("Database", "The sizes of xList and yList do not match.")
            return
        }

        val db = writableDatabase
        db.beginTransaction() // Start the transaction
            for (i in xList.indices) {
                val values = ContentValues().apply {
                    put(DatabaseSchema.COLUMN_X, xList[i])
                    put(DatabaseSchema.COLUMN_Y, yList[i])
                }
                val result = db.insert(DatabaseSchema.TABLE_NAME, null, values)
                if (result == -1L) {
                    Log.d(
                        "Database",
                        "Failed to insert coordinate: x = ${xList[i]}, y = ${yList[i]}"
                    )
                }
            }
        db.setTransactionSuccessful() // Set the transaction as successful
        db.endTransaction() // End the transaction
        db.close()
    }

    fun clearDatabase(){
        val db = writableDatabase
        db.execSQL("DELETE FROM ${DatabaseSchema.TABLE_NAME}")
        db.close()
    }

    fun getRowCount(): Int {
        val db = readableDatabase
        val cursor = db.rawQuery("SELECT COUNT(*) FROM ${DatabaseSchema.TABLE_NAME}", null)

        var count = 0
        if (cursor.moveToFirst()) {
            count = cursor.getInt(0) // Get the count from the first column
        }

        cursor.close()
        db.close()
        return count
    }
}