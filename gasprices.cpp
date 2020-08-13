/*
    gasprices.cpp
    problem from nicholia's book

    date    aug 12, 2020
    note    expects GasPrices.txt in the working directory
*/
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

//------------------------------------------------------------------------
class GasPrice 
{
    protected:
    int       _month ;
    int       _year ;
    float     _price ;

    void parse_data( const string &d ) { // expecting: "04-19-1993:1.079"
        parse_date( d.substr( 0, d.find( ":" ) )) ;
        _price = stof( d.substr( d.find( ":" )+1 )) ;
    }
    void parse_date( const string &d ) { // expecting: "04-19-1993"
        _month = stoi( d.substr( 0, d.find( "-" ) ));
        _year = stoi( d.substr( d.find_last_of( "-" )+1 ));
    }
      
    public:
    GasPrice() { _month = 0 ; _year = 0 ; _price = 0.0 ; }
    GasPrice( const GasPrice &c ) { *this = c ; }
    GasPrice( const string &date, float price_ ) { parse_date( date ) ; _price = price_ ; }
    GasPrice( const string &data_line ) { parse_data( data_line ) ; }

    GasPrice &operator= ( const GasPrice &c ) {
        if (this != &c) {
            _month = c._month ;
            _year = c._year ;
            _price = c._price ;
        }
        return *this;
    }

    int month() const { return _month ; }
    int year() const { return _year ; }
    float price() const { return _price ; }
} ; // class GasPrice

typedef vector<GasPrice>     GasPrice_vec ;

//------------------------------------------------------------------------
const int MAX_LINE = 256 ;

int load_data( const string &path, GasPrice_vec &prices ) 
{
    char  line[MAX_LINE] ;

    ifstream  inf ;
    inf.open( path ) ;
    if (!inf.is_open()) 
    {
        return -1 ;
    }

    while (inf) 
    {
        line[0] = '\0' ;
        inf.getline( line, MAX_LINE ) ;
        if (line[0] != '\0') 
        {
            prices.push_back( GasPrice( line ) ) ;
        }
    }
    return 0 ;
} // :: load_data

//------------------------------------------------------------------------
void find_yearly_avg( const GasPrice_vec &prices ) 
{
    map<int, pair<int, float>>  yrs ; // key: year  value: pair( count, sum )
    for (auto &x : prices)
    {
        yrs[ x.year() ].first++ ;
        yrs[ x.year() ].second += x.price() ;
    }
    for (auto &x : yrs) 
    {
        auto &data = x.second ;
        data.second /= data.first ;
        printf( "%d avg:  %4.2f \n", x.first, data.second ) ;
    }
} // :: find_yearly_avg

//------------------------------------------------------------------------
class PriceStats 
{
    protected:
      float  _lo ;
      float  _hi ;
      float  _avg ;
      float  _sum ;
      int    _cnt ;

    public:
    PriceStats() {
        _lo = _hi = _avg = _sum = 0.0 ;
        _cnt = 0 ;
    }
    void   operator<< ( const GasPrice &gp ) {
        _sum += gp.price() ;
        _cnt++ ;
        if (_cnt == 1) {
            _lo = _hi = gp.price() ;
        } else {
            _lo = min( _lo, gp.price() ) ;
            _hi = max( _hi, gp.price() ) ;
        }
        _avg = _sum / (float)_cnt ;
    }
    const char *report( char *d ) {
        snprintf( d, 255, "%5.2f - %5.2f [avg: %5.2f]", _lo, _hi, _avg ) ;
        return d ;
    }
    bool is_valid() const { return (_cnt > 0) ; }
} ; // class PriceStats

//------------------------------------------------------------------------
const char *months[] = { "yr", "jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec", NULL } ;

void find_yearly_stats( const GasPrice_vec &prices ) 
{
    // map( year, map( months, stats ))
    // note:  month 0 == yearly stats
    //
    map<int, map<int, PriceStats>>  monthly ; 
    char  buf[256] ;
    
    for (auto &x : prices)
    {
        monthly[x.year()][ 0 ] << x ;
        monthly[x.year()][ x.month() ] << x ;
    }
    for (auto &x : monthly) 
    {
        auto &yr = x.second ;
        printf( "%d %s \n", x.first, yr[0].report(buf) ) ;
        for (int i = 1; i <= 12; i++) 
        {
            if (yr[i].is_valid())
            {
                printf( "  %s  %s\n", months[i], yr[i].report(buf) ) ;
            }
        }
        printf( "\n" );
    }
} // :: find_yearly_stats

//------------------------------------------------------------------------
int main( int argc, const char *argv[] ) 
{
    GasPrice_vec  prices ;

    if (load_data( "GasPrices.txt", prices ) != 0)
    {
        printf( "error reading data file 'GasPrices.txt' \n");
        printf( "terminating" ) ;
        return -1 ;
    }

//    find_yearly_avg  ( prices ) ;
    find_yearly_stats( prices ) ;

    return 0 ;
} // :: main
