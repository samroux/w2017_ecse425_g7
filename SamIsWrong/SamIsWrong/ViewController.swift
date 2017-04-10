//
//  ViewController.swift
//  SamIsWrong
//
//  Created by Amr Guzlan on 2017-04-05.
//  Copyright © 2017 amro. All rights reserved.
//

import UIKit
import CoreBluetooth
import AWSMobileHubHelper




class ViewController: UIViewController, CBCentralManagerDelegate, CBPeripheralDelegate {
    
    var manager : CBCentralManager! // connect
    var peripheral : CBPeripheral! // interact
   
    var awsFileManager: AWSUserFileManager!
    
    var BEAN_NAME : String!
    var BEAN_SCRATCH_UUID  : CBUUID!
    var BEAN_SERVICE_UUID : CBUUID!
    
    var SAMPLE_SERVICE_UUID : CBUUID!
    var SAMPLE_CHAR_UUID : CBUUID!
    
    var ACC_SERVICE_UUID : CBUUID!
    var ACC_CHAR_UUID : CBUUID!
    var ACC_CHAR_X_UUID : CBUUID!
    var ACC_CHAR_Y_UUID : CBUUID!
    var ACC_CHAR_Z_UUID : CBUUID!
    var ACC_CHAR_AWS_UUID: CBUUID!
    
    var BUTTON_SERVICE_UUID : CBUUID!
    var BUTTON_CHAR_UUID : CBUUID!
    
    var WSAMPLE_SERVICE_UUID : CBUUID!
    var WSAMPLE_CHAR_UUID : CBUUID!
    
    
    var PDATA_SERVICE_UUID : CBUUID!
    var PDATA_CHAR_UUID : CBUUID!
    var PDATA_CHAR_X_UUID : CBUUID!
    var PDATA_CHAR_Y_UUID : CBUUID!
    var PDATA_CHAR_Z_UUID : CBUUID!
    
    
    var central : CBCentralManager!
    
    var timer = Timer()
    var buttonColour = UIColor.white.cgColor
    
    let fileName = "Readings.csv"
    var path : NSURL!
    
    
    var currentX = 0
    var currentY = 0
    var currentZ = 0
    
    var xValues = [Int]()
    var yValues = [Int]()
    var zValues = [Int]()
    
    var filteredXValues = [Int]()
    var filteredYValues = [Int]()
    var filteredZValues = [Int]()
    
    
    
    
    var writeToFile = false
    var contentsToWrite : String!
    
    
    var prefix : String!
    var marker : String!
    var downloadedData : String!
    
    
    @IBOutlet weak var scanLabel: UILabel!
    @IBOutlet weak var scanButton: UIButton!
    @IBAction func scan(_ sender: Any) {
        scanLabel.text = "(Scanning...)"
        scanLabel.textColor = UIColor.white
        central.scanForPeripherals(withServices: nil, options: nil)
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        createFile()
        setupBluetooth()
        setBackgroundColor()
        setupScanButton()
        loadContents()
        timer = Timer.scheduledTimer(timeInterval: 3.0, target: self, selector: #selector(animateButton), userInfo: nil, repeats: true)
    }
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        if central.state == .poweredOn {
            self.central = central
        } else {
            print("Bluetooth not available.")
        }
    }
    
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        peripheral.discoverServices(nil)
    }
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        for service in peripheral.services! {
            let thisService = service as CBService
            //print("The service is \(service.uuid.uuidString)")
            
            if service.uuid == ACC_SERVICE_UUID {
                
                // print ("Acc. Service (\(service.uuid.uuidString))")
            }
            else if service.uuid == WSAMPLE_SERVICE_UUID {
                
            }
            else if service.uuid == SAMPLE_SERVICE_UUID {
                // print ("Sample Service (\(service.uuid.uuidString))")
            }
            else {
                // print ("Undefined service (\(service.uuid.uuidString))")
            }
            
            peripheral.discoverCharacteristics(
                nil,
                for: thisService
            )
        }
        
        print (" ***** Data Output ***** ")
        
    }
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        for characteristic in service.characteristics! {
            
            let thisCharacteristic = characteristic as CBCharacteristic
            if thisCharacteristic.uuid == WSAMPLE_CHAR_UUID {
                if characteristic.uuid == WSAMPLE_CHAR_UUID {
                    var randomValue : UInt8 = 9
                    let dataToWrite = Data(bytes: &randomValue, count: MemoryLayout<UInt8>.size)
                    peripheral.writeValue(dataToWrite, for: characteristic, type: .withResponse)
                }
            }
            else{
                print("else")
            }
            self.peripheral.setNotifyValue(
                true,
                for: thisCharacteristic
            )
        }
    }
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        let device = (advertisementData as NSDictionary)
            .object(forKey: CBAdvertisementDataLocalNameKey)
            as? String
        
        if device != nil {
            print ("\(device!) detected")
        }
        
        if device != nil && device! == BEAN_NAME {
            scanLabel.text = "Connecting to SPICY !"
            scanLabel.textColor = UIColor.green
            buttonColour = UIColor.green.cgColor
            self.manager.stopScan()
            
            self.peripheral = peripheral
            self.peripheral.delegate = self
            
            manager.connect(peripheral, options: nil)
        }
    }
    func peripheral(_ peripheral: CBPeripheral, didWriteValueFor characteristic: CBCharacteristic, error: Error?) {
        var randomValue : UInt8 = 9
        let dataToWrite = Data(bytes: &randomValue, count: MemoryLayout<UInt8>.size)
        peripheral.writeValue(dataToWrite, for: characteristic, type: .withResponse)
    }
    
    
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        
        
        var value : UInt8 = 0;
        
        if characteristic.value != nil {
            characteristic.value!.copyBytes(to: &value, count: MemoryLayout<UInt8>.size)
            print(value)
            
        }
        else{
            print("This is null")
            //Do nothing Value will be identified as nil on print
        }
        
        
        var servicename : String = "None"
        var charname : String = "None"
        
        if characteristic.service.uuid == ACC_SERVICE_UUID {
            servicename = "Acc"
            if characteristic.uuid == ACC_CHAR_UUID {
                charname = "acc"
            }
            else if characteristic.uuid == ACC_CHAR_X_UUID{
                charname = "acc_x"
                currentX = Int(value)
            }
            else if characteristic.uuid == ACC_CHAR_Y_UUID{
                charname = "acc_y"
                currentY = Int(value)
            }else if characteristic.uuid == ACC_CHAR_AWS_UUID{
                if value == 1  && writeToFile == true  {
                    print("Writing")
                    xValues.append(currentX)
                    yValues.append(currentY)
                    zValues.append(currentZ)
                    //contentsToWrite.append("\(currentX),\(currentY),\(currentZ)\n")
                }
                else if value == 0 && writeToFile == true  {
                    print("stopped writing")
                    uploadToAWS()
                    writeToFile = false
                }else if value == 1 && writeToFile == false{
                    print("[imgwpirg")
                    writeToFile = true
                }
            }
                
                
            else if characteristic.uuid == ACC_CHAR_Z_UUID{
                charname = "acc_z"
                currentZ = Int(value)
            }
            
        }
        else if characteristic.service.uuid == SAMPLE_SERVICE_UUID {
            servicename = "Sample"
            if characteristic.uuid == SAMPLE_CHAR_UUID {
                charname = "data"
            }
        }
        else {
            servicename = "Undefined"
            charname = "Undefined"
        }
        
//        if writeToFile{
//            contentsToWrite.append("\(currentX),\(currentY),\(currentZ)\n")
//        }
    }
    func setupBluetooth(){
        BEAN_NAME  = "SPICY"
        let sampleServiceByteArray : [UInt8] = [0x02,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        let sampleCharacteristicByteArray : [UInt8] = [0xe2,0x3e,0x78,0xa0, 0xcf,0x4a, 0x11,0xe1, 0x8f,0xfc, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        
        let accServiceByteArray : [UInt8] = [0x03,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        let accCharacteristicByteArray : [UInt8] = [0xe3,0x3e,0x78,0xa0, 0xcf,0x4a, 0x11,0xe1, 0x8f,0xfc, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        let accXCharacteristicByteArray : [UInt8] = [0xe4,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        let accYCharacteristicByteArray : [UInt8] = [0xe5,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        let accZCharacteristicByteArray : [UInt8] = [0xe6,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        let accAWSCharacteristicByteArray : [UInt8] = [0xf3,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        
        let buttonServiceByteArray : [UInt8] = [0x04,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        let buttonCharacteristicByteArray : [UInt8] = [0xe7,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        
        let wSampleServiceByteArray : [UInt8] = [0x05,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        let wSampleCharacteristicByteArray : [UInt8] = [0xe8,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        
        let pDataServiceByteArray : [UInt8] = [0x03,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        let pDataCharacteristicByteArray : [UInt8] = [0xe3,0x3e,0x78,0xa0, 0xcf,0x4a, 0x11,0xe1, 0x8f,0xfc, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        let pDataXCharacteristicByteArray : [UInt8] = [0xe4,0x3e,0x78,0xa0, 0xcf,0x4a, 0x11,0xe1, 0x8f,0xfc, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        let pDataYCharacteristicByteArray : [UInt8] = [0xe5,0x3e,0x78,0xa0, 0xcf,0x4a, 0x11,0xe1, 0x8f,0xfc, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        let pDataZCharacteristicByteArray : [UInt8] = [0xe6,0x3e,0x78,0xa0, 0xcf,0x4a, 0x11,0xe1, 0x8f,0xfc, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        
        
        
        SAMPLE_SERVICE_UUID = CBUUID(string: NSUUID(uuidBytes: sampleServiceByteArray).uuidString)
        SAMPLE_CHAR_UUID = CBUUID(string: NSUUID(uuidBytes: sampleCharacteristicByteArray).uuidString)
        
        ACC_SERVICE_UUID =  CBUUID(string: NSUUID(uuidBytes: accServiceByteArray).uuidString)
        ACC_CHAR_UUID =  CBUUID(string: NSUUID(uuidBytes: accCharacteristicByteArray).uuidString)
        ACC_CHAR_X_UUID =  CBUUID(string: NSUUID(uuidBytes: accXCharacteristicByteArray).uuidString)
        ACC_CHAR_Y_UUID =  CBUUID(string: NSUUID(uuidBytes: accYCharacteristicByteArray).uuidString)
        ACC_CHAR_Z_UUID =  CBUUID(string: NSUUID(uuidBytes: accZCharacteristicByteArray).uuidString)
        ACC_CHAR_AWS_UUID = CBUUID(string: NSUUID(uuidBytes: accAWSCharacteristicByteArray).uuidString)
        
        
        BUTTON_SERVICE_UUID = CBUUID(string: NSUUID(uuidBytes: buttonServiceByteArray).uuidString)
        BUTTON_CHAR_UUID = CBUUID(string: NSUUID(uuidBytes: buttonCharacteristicByteArray).uuidString)
        
        WSAMPLE_SERVICE_UUID = CBUUID(string: NSUUID(uuidBytes: wSampleServiceByteArray).uuidString)
        WSAMPLE_CHAR_UUID = CBUUID(string: NSUUID(uuidBytes: wSampleCharacteristicByteArray).uuidString)
        
        
        PDATA_SERVICE_UUID =  CBUUID(string: NSUUID(uuidBytes: pDataServiceByteArray).uuidString)
        PDATA_CHAR_UUID =  CBUUID(string: NSUUID(uuidBytes: pDataCharacteristicByteArray).uuidString)
        PDATA_CHAR_X_UUID =  CBUUID(string: NSUUID(uuidBytes: pDataXCharacteristicByteArray).uuidString)
        PDATA_CHAR_Y_UUID =  CBUUID(string: NSUUID(uuidBytes: pDataYCharacteristicByteArray).uuidString)
        PDATA_CHAR_UUID =  CBUUID(string: NSUUID(uuidBytes: pDataZCharacteristicByteArray).uuidString)
        
        manager = CBCentralManager(delegate: self, queue: nil)
        
    }
    func setBackgroundColor(){
        let bc = CAGradientLayer()
        let topColor = UIColor(colorLiteralRed: 0.973, green: 0.631, blue: 0.290, alpha: 1.00).cgColor
        let bottomColor = UIColor(colorLiteralRed: 0.984, green: 0.286, blue: 0.212, alpha: 1.00).cgColor
        bc.colors = [topColor, bottomColor]
        bc.locations = [0.0, 1.0]
        bc.frame = self.view.bounds
        bc.zPosition = -1
        self.view.layer.insertSublayer(bc, at: 0)
        
    }
    func writeToPeripheral(){
        //    for service
    }
    func setupScanButton(){
        let borderColor = UIColor(red: 0.41, green: 1.28, blue: 1.85, alpha: 0.0)
        scanButton.layer.borderWidth = 4
        scanButton.layer.borderColor = borderColor.cgColor
        scanLabel.text = ""
        
    }
    func uploadToAWS(){
        do {
            for i in 0..<filteredXValues.count{
                contentsToWrite.append("\(filteredXValues[i]),\(filteredYValues[i]),\(filteredZValues[i])\n")
            }
            try contentsToWrite.write(to: path as URL, atomically: true, encoding: String.Encoding.utf8)
            let data  = contentsToWrite.data(using: String.Encoding.utf8)
            uploadWithData(data: data!, forKey: "readings.csv")
            
        } catch {
            print("Failed to create file")
            print("\(error)")
        }
    }
    func animateButton (){
        self.scanButton.layer.borderWidth = 3.0
        let color: CABasicAnimation = CABasicAnimation(keyPath: "borderColor")
        color.fromValue = UIColor.clear.cgColor
        color.toValue = buttonColour
        color.duration = 1.5
        color.autoreverses = true
        self.scanButton.layer.borderColor = UIColor.clear.cgColor
        self.scanButton.layer.add(color, forKey: "")
    }
    func createFile(){
        path = NSURL(fileURLWithPath: NSTemporaryDirectory()).appendingPathComponent(self.fileName)! as NSURL
        contentsToWrite = "X,Y,Z\n"
    }
    func applyFilter(){
        for i in 0..<(xValues.count-4){
            let num1 = 0.25*Double(xValues[i])
            let num2 = 0.25*Double(xValues[i+1])
            let num3 = 0.25*Double(xValues[i+2])
            let num4 = 0.25*Double(xValues[i+3])
            filteredXValues.append(Int(num1+num2+num3+num4))
        }
        for i in 0..<(yValues.count-4){
            let num1 = 0.25*Double(yValues[i])
            let num2 = 0.25*Double(yValues[i+1])
            let num3 = 0.25*Double(yValues[i+2])
            let num4 = 0.25*Double(yValues[i+3])
            filteredYValues.append(Int(num1+num2+num3+num4))
        }
        for i in 0..<(zValues.count-4){
            let num1 = 0.25*Double(zValues[i])
            let num2 = 0.25*Double(zValues[i+1])
            let num3 = 0.25*Double(zValues[i+2])
            let num4 = 0.25*Double(zValues[i+3])
            filteredZValues.append(Int(num1+num2+num3+num4))
        }
    }
    private func uploadWithData(data: Data, forKey key: String) {
        let manager = AWSUserFileManager.defaultUserFileManager()
        let localContent = manager.localContent(with: data, key: key)
        localContent.uploadWithPin(
            onCompletion: false,
            progressBlock: {[weak self](content: AWSLocalContent, progress: Progress) -> Void in
                guard let strongSelf = self else { return }
                /* Show progress in UI. */
            },
            completionHandler: {[weak self](content: AWSLocalContent?, error: Error?) -> Void in
                guard let strongSelf = self else { return }
                if let error = error {
                    print("Failed to upload an object. \(error)")
                } else {
                    print("Object upload complete. \(error)")
                }
        })
    }
    
    func loadContents(){
        let uploadsDirectory = "/"
        awsFileManager = AWSUserFileManager.defaultUserFileManager()
        awsFileManager.listAvailableContents(withPrefix: prefix, marker: marker) {[weak self] (contents: [AWSContent]?, nextMarker: String?, error: Error?) in
            guard let strongSelf = self else { return }
            if let error = error {
                print("Failed to load the list of contents. \(error)")
            }
            if let contents = contents, contents.count > 0 {
                for content in contents{
                    if content.key == "readings.csv"{
                        self?.downloadContent(content: content, pinOnCompletion: false)
                    }
                }
            }
        }
    }
    
   private func downloadContent(content: AWSContent, pinOnCompletion: Bool) {
    
    content.download(with: .always, pinOnCompletion: pinOnCompletion, progressBlock: {[weak self] (content: AWSContent, progress: Progress) in
        return
        
    }) {[weak self] (content: AWSContent?, data: Data?, error: Error?) in
        guard let strongSelf = self else { return }
        if let error = error {
            print("Failed to download a content from a server. \(error)")
           
        }else{
           
            let downloadedReadings = String(data: data!, encoding: String.Encoding.utf8)
            print(downloadedReadings!)
        }
    }
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
}

