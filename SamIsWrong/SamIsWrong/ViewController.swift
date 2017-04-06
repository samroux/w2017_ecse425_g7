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

    override func viewDidLoad() {
        super.viewDidLoad()
        BEAN_NAME  = "SPICY"
        let serviceByteArray : [UInt8] = [0x02,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
         let characteristicByteArray : [UInt8] = [0xe2,0x3e,0x78,0xa0, 0xcf,0x4a, 0x11,0xe1, 0x8f,0xfc, 0x00,0x02,0xa5,0xd5,0xc5,0x1b]
        BEAN_SCRATCH_UUID = CBUUID(string: NSUUID(uuidBytes: characteristicByteArray).uuidString)
        BEAN_SERVICE_UUID = CBUUID(string: NSUUID(uuidBytes: serviceByteArray).uuidString)
        
        manager = CBCentralManager(delegate: self, queue: nil)
        
    
        // Do any additional setup after loading the view, typically from a nib.
    
        
    }
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        if central.state == .poweredOn {
            central.scanForPeripherals(withServices: nil, options: nil)
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
            print("The service is \(service.uuid.uuidString)")

            //if service.uuid == BEAN_SERVICE_UUID {
                peripheral.discoverCharacteristics(
                    nil,
                    for: thisService
                )
            //}
        }
    }
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        for characteristic in service.characteristics! {
            let thisCharacteristic = characteristic as CBCharacteristic
            print("The following is a characteristic of \(service.uuid.uuidString)")
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
        
        print (device)
        if device != nil && device! == BEAN_NAME {
            
            self.manager.stopScan()
            
            self.peripheral = peripheral
            self.peripheral.delegate = self
            
            manager.connect(peripheral, options: nil)
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        var samAss : UInt8 = 0
        characteristic.value!.copyBytes(to: &samAss, count: MemoryLayout<UInt8>.size)
        print("The value is \(samAss)")
    }
        

    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
}

