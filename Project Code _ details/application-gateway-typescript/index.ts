import { Contract } from '@hyperledger/fabric-gateway';
import {Request,Response} from 'express';
import { main, getAllAssets, initLedger, createAsset, transferAssetAsync, readAssetByID } from './app';
const express = require('express');
const app = express();
const port = 80;
let contract:Contract;
app.listen(port, async () => {
    contract = await main();
    await initLedger(contract);
});
app.get('/get/:id?', async (req:Request,res:Response) => {
    const { id } = req.params;
    let result:JSON;
    if(id===undefined){
        result = await getAllAssets(contract);
    }
    else {
        result = await readAssetByID(contract, String(id));
    }
    res.json(result);
});
app.post('/create', async (req:Request,res:Response) => {
    const {id, type, size, owner} = req.body;
    await createAsset(contract, id, type, size, owner);
    res.send('Transaction is successfully committed');
});
app.post('/transfer', async (req:Request,res:Response) => {
    const {id, owner} = req.body;
    await transferAssetAsync(contract, id, owner);
    res.send('Owner Transferred');
});
