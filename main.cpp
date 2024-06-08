#include <bits/stdc++.h>
#include <queue>
#include <vector>
#include <fstream>

using namespace std;
int x1=0;
int x2=0;
int x3=0;
int x4=0;
int x5=0;
int x6=0;

// Function prototypes
int hex_to_dec(char c, bool sign=false); //convert hexadecimal character to integer
string decToHexa(int n); // convert integer to 8 bit hexadecimal

struct test{
    int opcode, dest_i, A, B, src, imm4 ,imm8, cond, pc,cond_i,ALU_OUTPUT,zero;
    string LI_imm8,ALU_OUT_string;
    test(){
        opcode = dest_i = A = B = src = imm4=imm8 = cond = pc = cond_i =ALU_OUTPUT= -1;
        ALU_OUT_string = LI_imm8="XX";
        zero=1;
    }
};

struct fetch{
    char w, x, y, z;
    int pc;
    fetch(){
        w=x=y=z='X';
        pc=-1;
    }
};
// bool done[5][256]={false};

int halt=0;
vector<string> DCache;
vector<string> ICache;
vector<string> RF;

vector<int> availability(16, 0); 

string LMD;
int PC=0;
int clo=0; // clock
int k=0;
int flag=0; // check for JMP or BEQZ

int main(){
    string x;
    ifstream ifile;

    // Take DCache, ICache, RF from input files and store them in vectors
    ifile.open("input/DCache.txt", ios::in);
    while(ifile>>x){
        DCache.push_back(x);
    }
    ifile.close();

    ifile.open("input/ICache.txt", ios::in);
    while(ifile>>x){
        ICache.push_back(x);
    }
    ifile.close();

    ifile.open("input/RF.txt", ios::in);
    while(ifile>>x){
        RF.push_back(x);
    }
    ifile.close();
    // Inputs taken
    int tot_ins=0, arith_ins=0, logic_ins=0, shift_ins=0, mem_ins=0, cont_ins=0, halt_ins=0, tot_stall=0, data_stall=0, cont_stall=0, li_ins=0;
    int cycles=0;
    float cpi;
    test t, memory, exec, writeback;
    fetch fet, f, empty;
    test emptiness;
    while(halt!=1){
        cycles++;
        // if(cycles==30) return 0;
        if(flag==1) cont_stall++; // increase control stalls
        

        if(writeback.opcode!=15 && halt!=1){
            
            writeback=memory; // forwarding
            memory=emptiness;
            if(writeback.opcode>=0 && writeback.opcode<=11){
                availability[writeback.dest_i]--;
                // cout<<"writeback.dest_i == "<<writeback.dest_i<<" "<<availability[writeback.dest_i]<<endl;
            }
                // cout<<"WB "<<writeback.pc<<" "<< writeback.opcode<<" "<<writeback.dest_i<<" "<<writeback.ALU_OUT_string<<endl;
            //cout<<1<<endl;
            switch (writeback.opcode)
            {
                case 0: // ADD
                case 1: // SUB
                case 2: // MUL
                case 3: // INC
                case 4: // AND
                case 5: // OR
                case 6: // XOR
                case 7: // NOT
                case 8: // SLLI
                case 9: // SLLR
                    // cout<<writeback.ALU_OUTPUT<<" check "<<decToHexa(writeback.ALU_OUTPUT);
                    RF[writeback.dest_i]=decToHexa(writeback.ALU_OUTPUT);
                    break;
                case 10: // LI
                    RF[writeback.dest_i]=writeback.ALU_OUT_string;
                    break;
                case 11: // LD
                    RF[writeback.dest_i]=LMD;
                    break;
                case 15: // HLT
                    halt=1;
                    break;
                default:
                    break;
            }
        }
        // for(int i=0;i<16;i++){
        //     cout<<RF[i]<<" ";
        // }
        // cout<<endl;
        // for(int i=0;i<16;i++){
        //     cout<<availability[i]<<" ";
        // }
        // cout<<endl;
        // cout<<" last_check "<<RF[8]<<endl;
        //cout<<2<<endl;


        // ########################################          MEM   #############
        if(memory.opcode!=15 ){
            
            memory=exec;
            if(memory.opcode==15){
                continue;
            }
            exec=emptiness;
            if(memory.opcode==13 || memory.opcode==14){
                flag=0;
            }
            // cout<<"MEM "<<memory.pc<<endl;
            // cout<<memory.ALU_OUTPUT<<endl;
            if(memory.opcode==10) LMD=memory.ALU_OUT_string;
            else if(memory.opcode==11) LMD = DCache[memory.ALU_OUTPUT];
            else if(memory.opcode==12) DCache[memory.ALU_OUTPUT] = RF[memory.dest_i];
            else if(memory.opcode==13){
                PC=memory.ALU_OUTPUT;
            }
            else if(memory.opcode==14 && memory.cond==0){
                PC=memory.ALU_OUTPUT;
            }
        }
        //EXEC
        if(exec.opcode!=15 && halt!=1){
            
            exec=t;
            if(exec.opcode==15){
                continue;
            }
            t=emptiness;
            // cout<<"EXEC "<<exec.pc<<endl;
            // if(exec.opcode<0) cout<<"bvkhdbfkjbekjfblrbfl4r"<<endl;
            if(exec.opcode>=0 && exec.opcode<=3) {
                arith_ins++;
                tot_ins++;
            }
            else if(exec.opcode>=4 && exec.opcode<=7) {
                logic_ins++;
                tot_ins++;
            }
            else if(exec.opcode==8 ||  exec.opcode==9) {
                shift_ins++;
                tot_ins++;
            }
            else if(exec.opcode==10){
                li_ins++;
                tot_ins++;
            }
            else if(exec.opcode==11 || exec.opcode==12) {
                mem_ins++;
                tot_ins++;
            }
            else if (exec.opcode==13 || exec.opcode==14) {
                cont_ins++;
                tot_ins++;
            }
            
            switch(exec.opcode){
                case 0: // ADD
                    exec.ALU_OUTPUT=exec.A+exec.B;
                    // cout<<" output_end == "<<exec.ALU_OUTPUT<<endl;
                    break;
                case 1: // SUB
                    exec.ALU_OUTPUT=exec.A-exec.B;
                    break;
                case 2: // MUL
                    t.ALU_OUTPUT=exec.A*exec.B;
                    break;
                case 3: // INC
                    exec.ALU_OUTPUT=exec.dest_i+1;
                    break;
                case 4: // AND
                    exec.ALU_OUTPUT=exec.A & exec.B;
                    break;
                case 5: // OR
                    exec.ALU_OUTPUT=exec.A | exec.B;
                    break;
                case 6: // XOR
                    exec.ALU_OUTPUT= exec.A ^ exec.B;
                    break;
                case 7: // NOT
                    exec.ALU_OUTPUT = ~exec.A;
                    break;
                case 8: // SLLI
                    exec.ALU_OUTPUT = exec.src << exec.imm4;
                    break;
                case 9: // SRLI
                    exec.ALU_OUTPUT = exec.src >> exec.imm4;
                    break;
                case 10: // LI
                    exec.ALU_OUT_string = exec.LI_imm8;
                    break;
                case 11: // LD
                case 12: // ST
                    // cout<<" end_check "<<exec.src<<" "<<exec.imm4<<endl;
                    exec.ALU_OUTPUT = exec.src + exec.imm4;
                    //   cout<<"srcccc ";
                    // cout<< t.imm4<<" "<<t.src<<" ";
                    break;
                case 13: // JMP
                    exec.ALU_OUTPUT = exec.pc + 2*exec.imm8+2;
                    break;
                case 14: // BEQZ
                    if(exec.cond==0){
                        exec.ALU_OUTPUT = exec.pc + 2*exec.imm8+2;
                    } 
                    // cout<<exec.ALU_OUTPUT<<" "<<exec.pc <<" "<<2*exec.imm8+2<<" "<<exec.cond<<endl;
                    break;
                case 15: // HLT
                  //  halt=1;
                    break;
                default:
                    break;
            }
        // cout<<3<<endl;
        }
        //INSTRUCTION DECODE
        if(t.opcode!=15 && halt!=1){
            //  cout<<4<<endl;
            f=fet;
            if(f.w==15){
                continue;
            }
            fet=empty;
            t.pc=f.pc;
            // cout<<"ID "<<t.pc<<endl;
            //   if(t.pc==14) k++;
            // if(k==2) return 0;
            // cout<<1<<endl;
            string s;
            int A_i;
            int B_i;
            int src_i;
            int cond_i;
            //  cout<<" w "<<f.w<<endl;
            switch(f.w){
                case '0': // ADD
                case '1': // SUB
                case '2': // MUL
                case '4': // AND
                case '5': // OR
                case '6': // XOR
                    t.opcode=hex_to_dec(f.w);
                    t.dest_i=hex_to_dec(f.x);
                    A_i=hex_to_dec(f.y);
                    B_i=hex_to_dec(f.z);

                    if(availability[A_i]>0 || availability[B_i]>0){
                        fet=f;
                        t=emptiness;
                        data_stall++;
                        x1++;
                        continue;
                        //  done[1][n]=false;
                    }
                    else{
                    // cout<<"abcd 1234"<<endl;
                    availability[t.dest_i]++;
                    // cout<<t.dest_i<<" dest "<<availability[t.dest_i]<<endl;
                    s=RF[A_i];
                    t.A=hex_to_dec(s[0], 1)*16 + hex_to_dec(s[1]);
                    s=RF[B_i];
                    t.B=hex_to_dec(s[0], 1)*16 + hex_to_dec(s[1]);
                    }
                    break;

                case '3': // INC

                    t.opcode=hex_to_dec(f.w);
                    t.dest_i=(hex_to_dec(f.x));
                    if(availability[t.dest_i]>1){
                        fet=f;
                        t=emptiness;
                        data_stall++;
                        x2++;
                        continue;
                    }
                    else{
                        availability[t.dest_i]++;
                    }

                    break;

                case '7': // NOT

                    t.opcode=hex_to_dec(f.w);
                    t.dest_i=hex_to_dec(f.x);
                    src_i=hex_to_dec(f.y);

                    if(availability[src_i]>0){
                        fet=f;
                        t=emptiness;
                        data_stall++;
                        x3++;
                        continue;
                    }
                    else{
                        availability[t.dest_i]++;

                        s=RF[src_i];
                        t.src=hex_to_dec(s[0])*16 + hex_to_dec(s[1]);
                    }
                    break;

                case '8': // SLLI
                case '9': // SRLI
                case 'b': // LD
                

                    t.opcode=hex_to_dec(f.w);
                    t.dest_i=hex_to_dec(f.x);
                    src_i=hex_to_dec(f.y);
                    t.imm4=hex_to_dec(f.z, true);
                    // cout<<f.y<<" ";
                    if(availability[src_i]>0){
                        fet=f;
                        t=emptiness;
                        data_stall++;
                        x4++;
                        continue;
                    }
                    else{
                        availability[t.dest_i]++;
                        s=RF[src_i];
                        t.src=hex_to_dec(s[0])*16 + hex_to_dec(s[1]);
                    }
                    break;
                case 'c': // ST
                
                    t.opcode=hex_to_dec(f.w);
                    t.dest_i=hex_to_dec(f.x);
                    src_i=hex_to_dec(f.y);
                    t.imm4=hex_to_dec(f.z);
                    s=RF[src_i];
                    t.src=hex_to_dec(s[0])*16 + hex_to_dec(s[1]);

                    // cout<<" ST "<<t.src<<endl;
                     //   t.dest_i=
                    //  cout<<src_i<<" "<<t.dest_i<<endl;
                    // cout<<" ST "<<s[0]<<" "<<s[1]<<" "<<hex_to_dec(s[0])*16<<" "<<hex_to_dec(s[1])<<" "<<t.src<<endl;

                    //  cout<<f.y<<" "<< availability[t.dest_i]<<" "<<availability[src_i]<<endl;
                    if(availability[t.dest_i]>0 || availability[src_i]>0){
                        data_stall++;
                        fet=f;
                        t=emptiness;
                        x5++;
                        //data_stall++;
                        continue;
                    }
                    //    availability[t.dest_i]++;
                    // }
                    break;
                case 'a': // LI

                    t.opcode=hex_to_dec(f.w);
                    t.dest_i=hex_to_dec(f.x);
                    t.LI_imm8="";
                    t.LI_imm8+=f.y;
                    t.LI_imm8+=f.z;
                    availability[t.dest_i]++;
                    break;


                case 'd': // JMP
                    flag=1;
                    t.opcode=hex_to_dec(f.w);
                    t.imm8=hex_to_dec(f.y)+16*hex_to_dec(f.x, true);
                    break;

                case 'e': // BEQZ
                    cond_i=hex_to_dec(f.x);
                    // cout<<availability[cond_i]<<" ";
                    if(availability[cond_i]>0){
                        fet=f;
                        t=emptiness;
                        data_stall++;x6++;

                        continue;
                    }
                    t.opcode=hex_to_dec(f.w);
                    t.imm8=hex_to_dec(f.z)+16*hex_to_dec(f.y, true);
                    // cout<<cond_i<<" "<<availability[cond_i]<<" BEQZ "<<endl;
                    t.cond=hex_to_dec(RF[cond_i][1])+hex_to_dec(RF[cond_i][0])*16;
                    flag=1;
                    break;

                case 'f': // HLT
                    t.opcode=hex_to_dec(f.w);
                 //   halt=1;
                    continue;
                    break;

                default:
                    break;
            }
        }
        // INSTRUCTION FETCH
        if(fet.w!='f' && flag!=1){
            // cout<<"IF "<<PC<<endl;
            fet.w=ICache[PC][0];
            fet.x=ICache[PC][1];
            fet.y=ICache[PC+1][0];
            fet.z=ICache[PC+1][1];
            fet.pc=PC;
            // fetch2=ICache[PC+1][0];
            PC+=2;
            // cout<<"kya hua?"<<endl;
        }
        clo+=2;
    }
    if(halt){
        halt_ins++;
        tot_ins++;
    }

    tot_stall=cont_stall+data_stall;
    cpi=1.0*cycles/tot_ins;
    
    // cout<<cycles<<endl;
    ofstream ofile;
    
    ofile.open("output/DCache.txt", ios::out);
    for(auto i : DCache){
        ofile<<i<<endl;
        // cout<<i<<endl;
    }
    ofile.close();
    
    ofile.open("output/Output.txt", ios::out);
    ofile
    <<"Total number of instructions executed        : "<<tot_ins<<endl
    <<"Number of instructions in each class"       <<endl
    <<"Arithmetic instructions                      : "<<arith_ins<<endl
    <<"Logical instructions                         : "<<logic_ins<<endl
    <<"Shift instructions                           : "<<shift_ins<<endl
    <<"Memory instructions                          : "<<mem_ins<<endl
    <<"Load immediate instructions                  : "<<li_ins<<endl
    <<"Control instructions                         : "<<cont_ins<<endl
    <<"Halt instructions                            : "<<halt_ins<<endl
    <<"Cycles Per Instruction                       : "<<cpi<<endl
    <<"Total number of stalls                       : "<<tot_stall<<endl
    <<"Data stalls (RAW)                            : "<<data_stall<<endl
    <<"Control stalls                               : "<<cont_stall<<endl;

    // ofile<<x1<<" "<<x2<<" "<<x3<<" "<<x4<<" "<<x5<<" "<<x6<<endl;
    ofile.close();
}
// ################################ End main ############################################

int hex_to_dec(char c, bool sign){
    if(!sign){
        if(c<='9' && c>='0') return (c-'0');
        else return (c-'a'+10);
    }
    if(c<='7' && c>='0') return c-'0';
    if(c=='8'|| c=='9') return c-'0'-16;
    return c-'a'+10-16;
}

string decToHexa(int n)
{
    // ans string to store hexadecimal number
    string ans = "";
   if(n<0) n+=256;

    while (n != 0) {
        // remainder variable to store remainder
        int rem = 0;
         
        // ch variable to store each character
        char ch;
        // storing remainder in rem variable.
        rem = n % 16;
 
        // check if temp < 10
        if (rem < 10) {
            ch = rem + 48;
        }
        else {
            ch = rem + 55;
        }
         
        // updating the ans string with the character variable
        ans += ch;
        n = n / 16;
    }
    int i = 0, j = ans.size() - 1;
    while(i <= j)
    {
      swap(ans[i], ans[j]);
      i++;
      j--;
    }
    while(ans.size()<2) ans="0"+ans;
    for(int i=0, k=ans.size(); i<k; i++){
        ans[i]=tolower(ans[i]);
    }
    return ans;
}