Switch Lampu Otomatis Beserta Jam Digital Berbasis Mikrokontroller AT89S51
===================

**Istantyo Triono: 13204102**

**Muhammad Zein: 13204116**

Abstrak
---------
Rangkaian yang dibuat merupakan implementasi dari jam digital yang dilengkapi timer untuk menyala-matikan lampu secara otomatis sesuai dengan nilai waktu nyala dan mati yang dimasukkan oleh user. Rangkaian dibuat dengan menggunakan mikrokontroller AT89S51, empat buah 7-Segment sebagai display waktu, beserta rangkaian lain sebagai pendukung. Digunakan rangkaian minimal dari mikrokontroller AT89S51 yang berfungsi untuk men-download program ke dalam mikrokontroller sebagai inti rangkaian. 7-Segment dinyalakan secara multiplexing dengan port 2 mikrokontroller sebagai penentu LED mana yang harus menyala. Data dikirimkan melalui port 1 mikrokontroller. Pemilihan mode dan penggantian nilai jam maupun menit dilakukan dengan dua buah switch yang terhubung ke port 3. Simulasi menunjukkan timer alat berjalan dengan baik. Namun implementasi hardware tidak berjalan baik. Port 2 yang berfungsi menentukan 7-Segment mana yang menyala tidak bekerja sehingga seluruh 7-Segment menyala bersamaan. Penyebabnya adalah penggunaan transitor NPN untuk men-drive 7-Segment. Setelah keempat transistor diganti dengan transistor PNP jam digital bekerja dengan baik. Apabila mikrokontroller yang digunakan dapat diganti dengan mikrokontroller yang lebih murah dan berukuran kecil, serta bila kemasan dari alat diperbaiki maka alat ini dapat dijual ke pasaran dengan nilai tambah yang menguntungkan.



Pendahuluan dan Latar Belakang
-------------

Sering kali ketika seseorang hendak meninggalkan rumah dalam jangka waktu yang lama ia akan menyalakan lampu depan rumah dengan harapan memberikan kesan seolah rumah tersebut berpenghuni. Tujuannya untuk mengurungkan niat pihak-pihak yang berniat jahat terhadap isi rumah tersebut, misalnya ingin mencuri. Namun langkah ini justru berbalik ketika lampu tersebut terus menyala pada pagi dan siang hari. Orang yang melihatnya justru seolah diberitahu bahwa rumah tersebut kosong, hal ini tentu saja dapat memancing niat buruk mereka. Untuk menghindari hal tersebut maka di rancanglah sebuah pengendali berbasis mikrokontroller yang dapat menyala-matikan lampu sesuai dengan waktu yang telah ditentukan sebelumnya oleh pengguna.

Desain Perangkat Keras
----------------------------
Inti dari sistem ini adalah sebuah mikrokontroller AT89S51 yang dipasang pada rangkaian minimum system untuk men-download program kedalam mikrokontroller. Crystal yang digunakan berfrekuensi 11,059 MHZ yang akan menghasilkan machine cycle kira-kira 0,9 usekon.

Untuk men-drive rangkaian 7-segment digunakan empat buah transistor PNP yang bagian Base-nya dihubungkan dengan Port 2 mikrokontroller. Gunanya sebagai penentu 7-Segment mana yang akan menyala.Tujuh buah pin pada Port 1 dihubungkan dengan keempat 7-Segment secara multiplexing untuk menentukan angka apa yang akan ditampilkan pada 7-segment. Pada Port 1 juga terpasang resistor dan led merah sebagai bagian dari minimum sistem rangkaian AT89S51.

Dua buah switch yang menentukan mode dari timer dan untuk mengganti nilai menit maupun jam dihubungkan ke port 3-6 dan 3-7. Digunakan pula dua buah LED merah kecil untuk menunjukkan mode waktu-on dan waktu-off, yaitu waktu ketika lampu menyala dan mati. Untuk mensimulasikan lampu digunakan sebuah LED merah yang berukuran lebih besar. Nantinya bagian ini akan dihubungkan ke relay yang akan menyala-matikan lampu sesuai dengan sinyal dari mikrokontroller.

Berikut adalah gambar desain perangkat keras yang digunakan sebagai simulasi dengan simulator ISIS Professional.

![enter image description here](13204102-13204116-Simulasi%20Proteus.JPG)

[Embeded \[CLOCK TIMER\].DSN](Embeded%20%5BCLOCK%20TIMER%5D.DSN)

Desain Perangkat Lunak
---------------------------

Input untuk menentukan mode serta angka yang di-display oleh 7-Segment menggunakan sistem scanning yang digabungkan dengan de-bouncing. Display pada 7-segment dilakukan secara multiplexing karena alasan terbatasnya jumlah port yang tersedia pada mikrokontroller. Pemakaian konsep multiplexing memungkinkan port yang digunakan sesedikit mungkin.

Scanning dan multiplexing dilakukan setiap 5 milisekon dengan menggunaan fitur timer0 pada arsitektur mikrokontroller 8051. Timer0 di-set pada mode 1 dengan nilai reload EE00. 
Digunakan delay waktu yang berbeda untuk memberikan efek berkedip pada 7-Segment.
Dibawah ini mode yang digunakan beserta fungsinya :
0 : Display waktu
1 : Mengganti menit. Bagian menit berkedip.
2 : Mengganti jam. Bagian jam berkedip.
3 : Mode waktu-on. Memasukkan nilai menit-on. Bagian menit berkedip
4 : Mode waktu-on. Memasukkan nilai jam-on. Bagian jam berkedip
5 : Mode waktu-off. Memasukkan nilai menit-off. Bagian menit berkedip.
6 : Mode waktu-off. Memasukkan nilai jam-off. Bagian jam berkedip
Switch pertama digunakan untuk memilih mode sementara switch kedua untuk mengganti nilai menit atau jam (tergantung pada modenya).
Implementasi Perangkat Keras


Rangkaian dasar yang digunakan adalah rangkaian minimal system untuk mikrokontroller berarsitektur 8051. Disini digunakan mikrokontroller AT89S51 keluaran Atmel. Rangkaian tambahannya adalah empat buah 7-Segment yang digunakan secara multipleks dengan menyatukan input kedelapan segmen serta common anode yang terpisah sehingga keempat 7-Segment dapat dioperasikan dengan menggunakan sebelas port saja. Common anode dihubungkan dengan Collector dari transistor PNP. Base transistor dihubungkan dengan port2 yang akan menentukan 7-Segmant mana yang akan menyala. Emitter dihubungkan dengan Vcc sebagai sumber tegangan bagi 7-Segment.

Implementasi Perangkat Lunak
-----------------------------------

Perangkat lunak dibuat dengan menggunakan bahasa C menggunakan program Keil Mikrovision3. Software ini lalu mengubah bahasa C tersebut menjadi file hex yang di-download kedalam mikrokontroller menggunakan software ISP Flash Programer 3.0a. Ini adalah source codenya : [Embeded \[CLOCK TIMER\].c](Embeded%20%5BCLOCK%20TIMER%5D.c)

Pengujian & Analisa
-----------------------

Setelah dilakukan simulasi, file-hex yang ada kemudian di-download ke dalam mikrrkontroller. Jam digital mampu menunjukkan angka yang menjadi default awal yaitu 00:00, jam 00, menit 00. Kemudian dengan menekan salah satu switch bagian jam dan menit berkedip dan dapat diganti dengan menekan switch lainnya.

Mode waktu-on dan waktu-off dapat diamati dengan melihat nyala LED merah kecil. Selain itu dengan memasukkan nilai waktu-on 00:01 dan waktu-off 00:02 fungsi penyala-matian lampu dapat dicek. Setelah satu menit dari reset, LED merah besar menyala yang menunjukkan fungsi waktu-on berhasil di-implementasikan. Satu menit kemudian LED merah besar tersebut mati yang berarti fungsi waktu-off bekerja.

![enter image description here](13204102-13204116-1113093856.JPG)
![enter image description here](13204102-13204116-1113093820.JPG)

Kesimpulan & Saran
-----------------------

Mikrokontroller AT89S51 dapat digunakan untuk aplikasi jam digital serta timer waktu. Aplikasi jam digital dengan memakai lebih dari satu 7-Segment mensyaratkan penggunaan metoda multiplexing guna menyalakan 7-Segment tersebut.
Debouncing input dapat diaplikasikan secara software maupun hardware dengan menggunakan kapasitor. Pada percobaan ini digunakan keduanya karena pada hardware minimum system telah tersedia fasilitas debouncing.
Tanpa transistor PNP, 7-Segment akan menyala amat redup karena arus yang keluar dari Port mikrokontroller terbagi menjadi delapan bagian, masing-masing bagian untuk menyalakan LED yang ada di 7-Segment. 
Permasalah yang cukup memusingkan dalam pembuatan alat ini adalah ketika 7-Segment terus menerus menyala tidak peduli berapapun nilai sinyal yang diberikan di Port2. Hal ini terjadi karena transistor yang digunakan adalah transistor NPN. Masalah ini selesai ketika transistor diganti menjadi transistor PNP.
