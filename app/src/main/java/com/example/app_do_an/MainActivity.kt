package com.example.app_do_an

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import android.widget.Button
import android.widget.ImageView
import android.widget.TextView
import android.widget.Toast
import com.google.firebase.database.DataSnapshot
import com.google.firebase.database.DatabaseError
import com.google.firebase.database.DatabaseReference
import com.google.firebase.database.FirebaseDatabase
import com.google.firebase.database.ValueEventListener

class MainActivity : AppCompatActivity(),View.OnClickListener {
    private lateinit var dbFirebase: DatabaseReference
    private lateinit var dbFirebase_d: DatabaseReference
    lateinit var txt_temp :TextView
    lateinit var txt_humid : TextView
    lateinit var txt_temp_f :TextView
    lateinit var txt_humid_f : TextView
    lateinit var btn:ImageView
    lateinit var btn_img:ImageView
    lateinit var btn_dv1:ImageView
    lateinit var btn_dv2:ImageView
    lateinit var btn_dv3:ImageView
    lateinit var btn_dv4:ImageView
    var img_d : List<Int> = listOf(R.drawable.icon_on,R.drawable._off)
    var device :MutableList<Int> = mutableListOf(0,0,0,0)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        var idBnt:List<Int> = listOf(R.id.btn_device1,R.id.btn_device2,R.id.btn_device3,R.id.btn_device4)
        txt_temp = findViewById(R.id.txt_temp_n)
        txt_humid = findViewById(R.id.txt_humi_n)
        btn_dv1 = findViewById(R.id.btn_device1)
        btn_dv2 = findViewById(R.id.btn_device2)
        btn_dv3 = findViewById(R.id.btn_device3)
        btn_dv4 = findViewById(R.id.btn_device4)

        getData_fb()
        idBnt.forEachIndexed { index, it ->
            val v = findViewById<View>(it)
            btn_img = findViewById<ImageView>(it)
            v.setOnClickListener(this)
            btn_img.setImageResource(img_d[device[index]])

        }
    }
    fun set_devive(){

    }
    override fun onClick(v:View) {
        btn = findViewById<ImageView>(v.id)
        when(v.id){
            R.id.btn_device1 -> kotlin.run { btn.setImageResource(img_d[device[0]])
                device[0] =(device[0] +1)%2
                saveData()
            }
            R.id.btn_device2 -> kotlin.run { btn.setImageResource(img_d[device[1]])
                device[1] =(device[1] +1)%2
                saveData()
            }
            R.id.btn_device3 -> kotlin.run { btn.setImageResource(img_d[device[2]])
                device[2] =(device[2] +1)%2
                saveData()
            }
            R.id.btn_device4 -> kotlin.run { btn.setImageResource(img_d[device[3]])
                device[3] =(device[3] +1)%2
                saveData()

            }

        }
    }
    private fun saveData() {

        val fb_data = data_device(device[0],device[1],device[2],device[3])

        Toast.makeText(this, "set valua", Toast.LENGTH_SHORT).show()

        dbFirebase_d.child("user1").setValue(fb_data).addOnCompleteListener {

            //Toast.makeText(this, "success", Toast.LENGTH_SHORT).show()

        }.addOnFailureListener {
            //Toast.makeText(this, "Error", Toast.LENGTH_SHORT).show()
        }
    }
    private fun getData_fb(){
        dbFirebase = FirebaseDatabase.getInstance().getReference("Data")
        dbFirebase.addValueEventListener(object : ValueEventListener {
            override fun onDataChange(snapshot: DataSnapshot) {
                if(snapshot.exists()){
                    for(dataFb in snapshot.children){
                        val data = dataFb.getValue(outData ::class.java)
                        txt_temp.text = data!!.temp
                        txt_humid.text = data!!.hummid
                    }
                }
            }

            override fun onCancelled(error: DatabaseError) {
                TODO("Not yet implemented")
            }
        })
        dbFirebase_d = FirebaseDatabase.getInstance().getReference("Data_device")
        dbFirebase_d.addValueEventListener(object : ValueEventListener {
            override fun onDataChange(snapshot: DataSnapshot) {
                if(snapshot.exists()){
                    for(dataF in snapshot.children){
                        val data_d = dataF.getValue(data_device ::class.java)
                        device[0] =(data_d!!.dv1)!!.toInt()
                        btn_dv1.setImageResource(img_d[device[0]])
                        device[1] =(data_d!!.dv2)!!.toInt()
                        btn_dv2.setImageResource(img_d[device[1]])
                        device[2] =(data_d!!.dv3)!!.toInt()
                        btn_dv3.setImageResource(img_d[device[2]])
                        device[3] =(data_d!!.dv4)!!.toInt()
                        btn_dv4.setImageResource(img_d[device[3]])
                    }
                }
            }
            override fun onCancelled(error: DatabaseError) {

            }
        })
    }
}
