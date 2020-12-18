//screw this cpp shit

import {exit} from "process";
import * as Combinatorics from "js-combinatorics";
import fs from "fs";

var argv = process.argv.slice(2);
//n: number of car types
//m: number of features
//q: max q for a feature
//maxn: maximum number of cars of each type;

let n=0;
let m=0;
let maxq=0;
let maxn=0;
let outputName = "instance.txt"
try{
    n=Number(argv[0]);
    m=Number(argv[1]);
    maxq=Number(argv[2]);
    maxn=Number(argv[3]);
    outputName=String(argv[4])
    
    if(n<=0 || m<=0 || maxq<=0 || maxn<=0 || isNaN(maxn) || isNaN(n) || isNaN(m) || isNaN(maxq)){
        throw "please select n, m, maxq and maxn strictly positive integers";
    }
}catch(e){
    console.error("please run node cars.js n m maxq maxn  (output name)");
    console.error(e);
    exit();
}

let asciiCode = 65;
let cars = {};
let features =[];//features[i] = {p: pi, q : qi}
let totalcars=0;
for(let i=0; i<n; i++){
    //how many cars of this type do we have to produce
    let qtt = Math.floor(Math.random()*maxn+1);
    cars[String.fromCharCode(asciiCode)]={"qtt":qtt, "feat":[]};
    totalcars = totalcars + qtt;
    for(let j=0; j<m; j++){
        //which features are required on this type of car
        if(Math.floor(Math.random()*2)==1){
            cars[String.fromCharCode(asciiCode)]["feat"].push(j);
        }
    }
    asciiCode++;
}
for(let i=0; i<m; i++){
    let q = Math.floor(Math.random()*maxq+1);
    let p = Math.floor(Math.random()*q+1);
    features.push({"p": p, "q":q})
}
/*
console.log(totalcars+" vehicles")
console.log('cars:')
console.log(cars)
console.log("\n")

console.log('features')
console.log(features)

console.log("\n")
*/

//generate the primary items, secondary items and options
let primaries = {};
let secondaries = {};


let primaryIndex = 0;
let secondaryIndex =0;
for(let [type, props] of Object.entries(cars)){
    for(let i=0; i<props.qtt; i++){
        primaries[type+i]=primaryIndex;
        primaryIndex++;       
    }
}
for(let j=0; j<totalcars; j++){
    //positions possibles sur la chaine
    //let pos = j;

    for(let k=0; k<features.length; k++){
        //features requises la voiture
        secondaries["x("+j+","+k+")"]=secondaryIndex;
        secondaryIndex++;
    }
}

let options = [];
//increase the primary index by the length of secondaries
for(let key of Object.keys(primaries)){
    primaries[key]+= secondaryIndex;
}
for(let [type, props] of Object.entries(cars)){

    for(let i=0; i<props.qtt; i++){
        for(let j=0; j<totalcars; j++){
            //positions possibles sur la chaine
            //let pos = j;
            let subitems = [];

            for(let k=0; k<props.feat.length; k++){
                //features requises la voiture
                let featId = props.feat[k];
                let required = secondaries["x("+j+","+featId+")"]+":1";
                let consequences = []
                for(let l=0; l<features[featId].q -1; l++){
                    if(j+l+1<totalcars){// no constraint after the chain length
                        consequences.push(secondaries["x("+String(j+l+1)+","+featId+")"]+":0");
                    }
                }
                let combinations = Combinatorics.Combination.of(consequences,(features[featId].q - features[featId].p));
                //console.log("combinations for feature "+featId+" on vehicule "+type+i+" at position "+j)
                //console.log([...combinations]);
                let comb =[...combinations]
                let subs = [];
                for(let c of comb){
                    //console.log(c)
                    subs.push(required+" "+c.join(" "))
                }
                if(comb.length===0){
                    subs.push(required)
                }
                //console.log("combinations for feature "+featId+" on vehicule "+type+i+" at position "+j)
                //console.log(subs)
                subitems.push(subs);
            }
            //console.log("subitems for vehicle "+type+i+" at position "+j)
            //console.log(subitems)
            //now we need to generate the color part of the item option by making all combinations of the subitems

            let itterating = [primaries[type+i]+" "];
            for(let subs of subitems){
                let tmp = [];
                for(let sub of subs){
                    for(let itteration of itterating){
                        tmp.push(itteration+" "+sub)
                    }
                }
                itterating = tmp;
            }               
            options.push(itterating);

        }

    }
    
}
options = options.flat();
/*console.log(options);
console.log("\n")
console.log(primaries);
    console.log(secondaries)
    console.log("\n")
    console.log(totalcars+" vehicles")
    console.log('cars:')
    console.log(cars)
    console.log("\n")*/
    //write head (nb of items + nb of secondaries + nb of options)
    let text = String(Object.keys(primaries).length+Object.keys(secondaries).length)+" "+Object.keys(secondaries).length+" "+options.length+"\n";
    //append the list of secondaries
    let orderedSec=[];
    for(let [key, val] of Object.entries(secondaries)){
        orderedSec.push({key, val});
    }
    orderedSec.sort((a, b)=> a.val-b.val)
    for(let obj of orderedSec){
        text+= obj.val+"\n";
    }
    //then append the list of primaries
    let orderedPrim=[];
    for(let [key, val] of Object.entries(primaries)){
        orderedPrim.push({key, val});
    }
    orderedPrim.sort((a, b)=> a.val-b.val)
    for(let obj of orderedPrim){
        text+= obj.val+"\n";
    }
    for(let opt of options){
        text+= opt+"\n";
    }
    fs.writeFile(outputName, text, function (err) {
        if (err) throw err;
        console.log('Saved!');
    });