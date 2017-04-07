//
//  ViewController.swift
//  SamIsWrong
//
//  Created by Amr Guzlan on 2017-04-05.
//  Copyright © 2017 amro. All rights reserved.
//

import UIKit
import CoreBluetooth




class ViewController: UIViewController, CBCentralManagerDelegate, CBPeripheralDelegate {
    
    var manager : CBCentralManager! // connect
    var peripheral : CBPeripheral! // interact 
    
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
    
    var BUTTON_SERVICE_UUID : CBUUID!
    var BUTTON_CHAR_UUID : CBUUID!
    
    var WSAMPLE_SERVICE_UUID : CBUUID!
    var WSAMPLE_CHAR_UUID : CBUUID!
    
    
    var PDATA_SERVICE_UUID : CBUUID!
    var PDATA_CHAR_UUID : CBUUID!
    var PDATA_CHAR_X_UUID : CBUUID!
    var PDATA_CHAR_Y_UUID : CBUUID!
    var PDATA_CHAR_Z_UUID : CBUUID!
    

    override func viewDidLoad() {
        super.viewDidLoad()
        BEAN_NAME  = "SPICY"
        let sampleServiceByteArray : [UInt8] = [0x02,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        let sampleCharacteristicByteArray : [UInt8] = [0xe2,0x3e,0x78,0xa0, 0xcf,0x4a, 0x11,0xe1, 0x8f,0xfc, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        
        let accServiceByteArray : [UInt8] = [0x03,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        let accCharacteristicByteArray : [UInt8] = [0xe3,0x3e,0x78,0xa0, 0xcf,0x4a, 0x11,0xe1, 0x8f,0xfc, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        let accXCharacteristicByteArray : [UInt8] = [0xe4,0x3e,0x78,0xa0, 0xcf,0x4a, 0x11,0xe1, 0x8f,0xfc, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        let accYCharacteristicByteArray : [UInt8] = [0xe5,0x3e,0x78,0xa0, 0xcf,0x4a, 0x11,0xe1, 0x8f,0xfc, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        let accZCharacteristicByteArray : [UInt8] = [0xe6,0x3e,0x78,0xa0, 0xcf,0x4a, 0x11,0xe1, 0x8f,0xfc, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        
        let buttonServiceByteArray : [UInt8] = [0x04,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        let buttonCharacteristicByteArray : [UInt8] = [0xe7,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        
        let wSampleServiceByteArray : [UInt8] = [0x05,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        let wSampleCharacteristicByteArray : [UInt8] = [0xe8,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        
        let pDataServiceByteArray : [UInt8] = [0x03,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        let pDataCharacteristicByteArray : [UInt8] = [0xe3,0x3e,0x78,0xa0, 0xcf,0x4a, 0x11,0xe1, 0x8f,0xfc, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        let pDataXCharacteristicByteArray : [UInt8] = [0xe4,0x3e,0x78,0xa0, 0xcf,0x4a, 0x11,0xe1, 0x8f,0xfc, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        let pDataYCharacteristicByteArray : [UInt8] = [0xe5,0x3e,0x78,0xa0, 0xcf,0x4a, 0x11,0xe1, 0x8f,0xfc, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        let pDataZCharacteristicByteArray : [UInt8] = [0xe6,0x3e,0x78,0xa0, 0xcf,0x4a, 0x11,0xe1, 0x8f,0xfc, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]

        
//        BEAN_SCRATCH_UUID = CBUUID(string: NSUUID(uuidBytes: characteristicByteArray).uuidString)
//        BEAN_SERVICE_UUID = CBUUID(string: NSUUID(uuidBytes: serviceByteArray).uuidString)
        
        SAMPLE_SERVICE_UUID = CBUUID(string: NSUUID(uuidBytes: sampleServiceByteArray).uuidString)
        SAMPLE_CHAR_UUID = CBUUID(string: NSUUID(uuidBytes: sampleCharacteristicByteArray).uuidString)
        
        ACC_SERVICE_UUID =  CBUUID(string: NSUUID(uuidBytes: accServiceByteArray).uuidString)
        ACC_CHAR_UUID =  CBUUID(string: NSUUID(uuidBytes: accCharacteristicByteArray).uuidString)
        ACC_CHAR_X_UUID =  CBUUID(string: NSUUID(uuidBytes: accXCharacteristicByteArray).uuidString)
        ACC_CHAR_Y_UUID =  CBUUID(string: NSUUID(uuidBytes: accYCharacteristicByteArray).uuidString)
        ACC_CHAR_UUID =  CBUUID(string: NSUUID(uuidBytes: accZCharacteristicByteArray).uuidString)
        
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
        
    
        // Do any additional setup after loading the view, typically from a nib.
    
        
    }
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        if central.state == .poweredOn {
            central.scanForPeripherals(withServices: nil, options: nil)
            print ("Scanning for devices...")
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
                print ("Acc. Service (\(service.uuid.uuidString))")
            }
            else if service.uuid == SAMPLE_SERVICE_UUID {
                print ("Sample Service (\(service.uuid.uuidString))")
            }
            else {
                print ("Undefined service (\(service.uuid.uuidString))")
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
            //print("The following is a characteristic of \(service.uuid.uuidString)")
            //print ("The charact \(thisCharacteristic.value)")
        
      
            //if thisCharacteristic.uuid == BEAN_SCRATCH_UUID {
                self.peripheral.setNotifyValue(
                    true,
                    for: thisCharacteristic
                )
           // }
                //print ("\(thisCharacteristic)")
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
            print ("Connecting to \(device!)...")
            
            self.manager.stopScan()
            
            self.peripheral = peripheral
            self.peripheral.delegate = self

            manager.connect(peripheral, options: nil)
        }
    }
    func peripheral(_ peripheral: CBPeripheral, didWriteValueFor characteristic: CBCharacteristic, error: Error?) {
        print ("didWriteValueFor")
        var randomValue : UInt8 = 9
        let dataToWrite = Data(bytes: &randomValue, count: MemoryLayout<UInt8>.size)
        peripheral.writeValue(dataToWrite, for: characteristic, type: .withResponse)
    }
    
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        
        //print ("didUpdateValueFor char")
        var value : UInt8 = 0;
        
        if characteristic.value != nil {
            characteristic.value!.copyBytes(to: &value, count: MemoryLayout<UInt8>.size)
        }
        else{
            //Do nothing Value will be identified as nil on print
        }
        
        //print ("Bytes Copied")
        
        //let data = characteristic.value!
        
        //let value_int : Double = data.withUnsafeBytes { $0.pointee }
        
        var servicename : String = "Undefined"
        var charname : String = "data"
        
        if characteristic.service.uuid == ACC_SERVICE_UUID {
            servicename = "Acc"
            if characteristic.uuid == ACC_CHAR_UUID {
                charname = "acc"
            }
            else if characteristic.uuid == ACC_CHAR_X_UUID{
                charname = "acc_x"
            }
            else if characteristic.uuid == ACC_CHAR_Y_UUID{
                charname = "acc_y"
            }
            else if characteristic.uuid == ACC_CHAR_Z_UUID{
                charname = "acc_z"
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
            charname = "data"
        }
        
        
        print("\(servicename).\(charname) = \(value) = \(characteristic.value)")
        //print("\(servicename).\(charname)")
    }
        

    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
}

