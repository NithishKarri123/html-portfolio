#include <bits/stdc++.h>
using namespace std;


unsigned int w = 32;
unsigned int r = 20;
unsigned int b ;
unsigned int log_w = (unsigned int)log2(w);
int64_t  modulo = pow(2, w);
vector<int> S(2*r+4);

string little_ndian(string str){
  string endian;
  
  if(str.length() % 2 == 0)
  {
    for(string::reverse_iterator it = str.rbegin();it != str.rend();it = it + 2)
    {
      endian.push_back(*(it+1));
      endian.push_back(*it);
    }
  }
  else
  {
    str = "0" + str;
    for(string::reverse_iterator it = str.rbegin();it != str.rend();it = it + 2)
    {
      endian.push_back(*(it+1));
      endian.push_back(*it);
    }
  }

  return endian;
}


// string hex_to_string(unsigned int A, unsigned int B, unsigned int C, unsigned int D)
// {

//   string strA, strB, strC, strD, result;

//   stringstream ss;
//   ss << setfill('0') << setw(8) << hex << A;
//   strA = little_ndian(ss.str());
//   ss.str("");
//   ss.clear();

//   ss << setfill('0') << setw(8) << hex << B;
//   strB = little_ndian(ss.str());
//   ss.str("");
//   ss.clear();
  
//   ss << setfill('0') << setw(8) << hex << C;
//   strC = little_ndian(ss.str());
//   ss.str("");
//   ss.clear();

//   ss << setfill('0') << setw(8) << hex << D;
//   strD = little_ndian(ss.str());
//   ss.str("");
//   ss.clear();

//   result = strA + strB + strC + strD;

//   return result;

// }

// Helper function to format an unsigned int as a hexadecimal string with a specified width
string formatHex(unsigned int value, int width) 
{
    string hexStr;
    while (value > 0 || width > 0) {
        int digit = value & 0xF;
        hexStr = "0123456789ABCDEF"[digit] + hexStr;
        value >>= 4;
        width--;
    }
    return hexStr;
}

string hex_to_string(unsigned int A, unsigned int B, unsigned int C, unsigned int D) 
{
    string strA, strB, strC, strD, combined;

    // Convert each unsigned int to a little-endian hexadecimal string
    strA = little_ndian(formatHex(A, 8));
    strB = little_ndian(formatHex(B, 8));
    strC = little_ndian(formatHex(C, 8));
    strD = little_ndian(formatHex(D, 8));

    // Concatenate the results
    combined = strA + strB + strC + strD;

    return combined;
}



int left_rotate(unsigned int a, unsigned int b, unsigned int w)
{
  return (a << b) | (a >> (w - b));  
}


int right_rotate(unsigned int a, unsigned int b, unsigned int w)
{
  return (a >> b) | (a << (w - b));
}



void key_schedule(string key)
{
  const unsigned int w_bytes = ceil((float)w / 8);
  const unsigned int c = ceil((float)b / w_bytes);

  unsigned int P32, Q32;
  P32=0xB7E15163;
  Q32=0x9E3779B9;

  vector<int> L(c);

  for(int i = 0; i < c; i++)
  {
    L[i] = strtoul(little_ndian(key.substr(w_bytes * 2 * i, w_bytes * 2)).c_str(), NULL, 16);
  }  

  S[0] = P32;
  for(int i = 1; i <= (2 * r + 3); i++)
  {
    S[i] = (S[i - 1] + Q32) % modulo;
  }

  unsigned int A = 0, B = 0, i = 0, j = 0;
  int v = 3 * max(c, (2 * r + 4));
  for(int f = 1; f <= v; f++)
  {
    A = S[i] = left_rotate((S[i] + A + B) % modulo, 3, w);
    B = L[j] = left_rotate((L[j] + A + B) % modulo, (A + B), w);
    i = (i + 1) % (2 * r + 4);
    j = (j + 1) % c;
  }
}


string encrypt(const string &text){
  string result;
  
  unsigned int A, B, C, D;
  A = strtoul(little_ndian(text.substr(0, 8)).c_str(), NULL, 16);
  B = strtoul(little_ndian(text.substr(8, 8)).c_str(), NULL, 16);
  C = strtoul(little_ndian(text.substr(16, 8)).c_str(), NULL, 16);
  D = strtoul(little_ndian(text.substr(24, 8)).c_str(), NULL, 16);

  int32_t t, u, temp;

  B += S[0];
  D += S[1];
  for(int i = 1; i <= r; ++i){
    t = left_rotate((B * (2 * B + 1)) % modulo, log_w, w);
    u = left_rotate((D * (2 * D + 1)) % modulo, log_w, w);
    A = left_rotate((A ^ t), u , w) + S[2 * i];
    C = left_rotate((C ^ u), t, w) + S[2 * i + 1];
    temp = A;
    A = B;
    B = C;
    C = D;
    D = temp;
  }

  A += S[2 * r + 2];
  C += S[2 * r + 3];

  result = hex_to_string(A, B, C, D);

  return result;
}


string decrypt(const string &text){
  string result;
  
  unsigned int A, B, C, D;
  A = strtoul(little_ndian(text.substr(0, 8)).c_str(), NULL, 16);
  B = strtoul(little_ndian(text.substr(8, 8)).c_str(), NULL, 16);
  C = strtoul(little_ndian(text.substr(16, 8)).c_str(), NULL, 16);
  D = strtoul(little_ndian(text.substr(24, 8)).c_str(), NULL, 16);

  unsigned int t, u, temp;  
  
  C -= S[2 * r + 3];
  A -= S[2 * r + 2];
  for(int i = r; i >= 1; --i){
    temp = D;
    D = C;
    C = B;
    B = A;
    A = temp;
    u = left_rotate((D * (2 * D + 1)) % modulo, log_w, w);
    t = left_rotate((B * (2 * B + 1)) % modulo, log_w, w);
    C = right_rotate((C - S[2 * i + 1]) % modulo, t, w) ^ u;
    A = right_rotate((A - S[2 * i]) % modulo, u, w) ^ t;
  }
  D -= S[1];
  B -= S[0];

  result = hex_to_string(A, B, C, D);

  return result;
}

string stringToHex(const string& input, size_t desiredLength) {
    stringstream hexStream;
    hexStream << std::hex << std::uppercase << std::setfill('0');

    for (char ch : input) {
        hexStream << std::setw(2) << static_cast<int>(static_cast<unsigned char>(ch));
    }

    string hexRepresentation = hexStream.str();

    // Pad with zeros to achieve the desired length
    if (hexRepresentation.length() < desiredLength) {
        hexRepresentation = std::string(desiredLength - hexRepresentation.length(), '0') + hexRepresentation;
    }

    return hexRepresentation;
}

string hexToString(string& hexString)
 {
    string result;
    istringstream iss(hexString);

    // Read two characters at a time and convert them to a byte
    for (size_t i = 0; i < hexString.length(); i += 2) 
    {
        string byteString = hexString.substr(i, 2);
        char byte = static_cast<char>(stoi(byteString, nullptr, 16));
        result += byte;
    }

    return result;
}


int main()
{
  string mode;
  cout<<"Enter the mode : ";
  cin>>mode;
  
  string text;
  if(mode=="Encryption")
     cout<<"Enter the plaintext : ";
  else
     cout<<"Enter the ciphertext : ";
  cin>>text;
  

  string key;
  cout<<"Enter the key : ";
  cin>>key;
  
  b=key.size()/2;
  
  string result;

  key_schedule(key);
  
  if(mode =="Encryption")
  {
    string encryption = encrypt(text);
    for(string::iterator it = encryption.begin(); it != encryption.end();it = it + 2)
    {
      result.push_back(*it);
      result.push_back(*(it+1));
      result = result + " ";
    }

    result= hexToString(result);
    cout<<"ciphertext is : "<< result << endl;
  }
  else if(mode == "Decryption")
  {
    string decryption = decrypt(text);
    for(string::iterator it = decryption.begin(); it != decryption.end();it = it + 2)
    {
      result.push_back(*it);
      result.push_back(*(it+1));
      result = result + " ";
    }
    cout<<"planetext is : "<< result << endl;
  }
  
  return 0;
 
}