# C.H.I.M.E-transmitter-verification
Research project to verify the MAX41460 planned to use for a radio-telescope calibrator. Chip should be able to output FM and AM signal.
Used platformIO as an IDE, burst SPI writes 17 registers then continously sends hex AA. To further the project, provided a schematic for FHSS signal:


Set up:

![image](https://user-images.githubusercontent.com/32754336/114271700-99095780-99c7-11eb-8e4b-578389c29b80.png)

Results is recorded with an SNA.

![image](https://user-images.githubusercontent.com/32754336/114271707-a292bf80-99c7-11eb-8efc-16d602dbf990.png)

![image](https://user-images.githubusercontent.com/32754336/114271711-a6264680-99c7-11eb-96ae-dffb79bc8c58.png)





The proposed FHSS signal includes the MAX41460 chip with an additional mixer and an LFSR implemented using an XNOR chip and a shift register: Schematic is provided below

![image](https://user-images.githubusercontent.com/32754336/114274163-0cb06200-99d2-11eb-81b2-3736f52ef8eb.png)
![image](https://user-images.githubusercontent.com/32754336/114274172-11751600-99d2-11eb-9256-a568794f98f4.png)
