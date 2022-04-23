1. tar -zxvf crc.tar.gz

2. g++ -o crc_encoder_20200001 crc_encoder_20200001.cc <br>
   g++ -o crc_decoder_20200001 crc_decoder_20200001.cc
   
3. ./crc_encoder datastream.tx codedstream.tx 1101 4<br>
   ./linksim codedstream.tx codedstream.rx 0.05 1001<br>
   ./crc_decoder codedstream.rx datastream.rx result.txt 1101 4
