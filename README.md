# w2017_ecse426_g7

## Project Specifications
#### 1. STM Discovery board
• Submits accelerometer readings to the AWS cloud, through the Nucleo board and
smartphone.
• Reads data obtained from AWS cloud through Nucleo board and outputs it on the DAC.

-> Get Accelerometer Readings (RAW)

-> Send RAW data to Nucleo using UART

-> Send push button status to Nucleo using UART

-> Receive Processed Data from Nucleo using UART

-> Output processed Accelerometer Data on DAC

#### 2. STM Nucleo board with BLE board
• Interconnects Discovery board with smartphone. Provides BLE functionality to the
Discovery board.

-> Receive RAW accelerometer data from Discovery Board using UART

-> Send data to smartphone using bluetooth
    -> 3 Services
        -> Accelerometer
            -> 3 Characterictics
                -> x_axis
                -> y_axis
                -> z_axis
        -> Button
        -> Processed Data
            -> 3 Characterictics
              -> x_axis
              -> y_axis
              -> z_axis

-> Receive processed data from smartphone using bluetooth

-> Send processed data to Discovery board using UART

#### 3. Smartphone
• Interconnects Nucleo and Discovery board with AWS cloud. Accesses cloud services, including the authentication. Sends processed data back to Nucleo board

-> Receive RAW accelerometer data from Nucleo using Bluetooth

-> Send raw data to aws

-> Send processed data to aws (if not using lambda)

-> Make Data available on phone display


#### 4. AWS cloud
• Stores Discovery board data, manipulated files, makes data visible to clients on any
platform. Basic processing includes operations such as mirror-imaging of the data.
• Bonus: Performs advanced processing (such as filtering, computing pitch and roll) of the
data using Lambda functions.

-> Responds to file upload requests (AWS is the web service repsonding)

-> Store raw data files (in S3)

-> Performs lambda (optional) processing on raw data

-> Stores processed file (in S3)

-> Responds to download request (AWS takes care of this)
