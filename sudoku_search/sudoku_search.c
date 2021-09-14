// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //

// sudoku search solver.

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdnoreturn.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <time.h>

// TODO these settings could be changed:
const static char VERSION[] = "2.1"; // previous was 2.0, next release ?
//#define DEBUG // TODO activate for massive stdout printing, no impact on output file
const static double RUN_TIME = 30.0;// roughly runs for this time, in seconds
const static unsigned N_MAX = 1000; // Number of tries for each sudoku puzzle


#define BLOCK_SIZE 3
#define GRID_SIZE (BLOCK_SIZE*BLOCK_SIZE) // help from cboard forum

const static char input_sep='/';
const static char output_sep='$';
#define LINE_LEN 126
#define PRE_ALLOC 100

#define BUFF_SIZE 256
#define INFO_BUFF_SIZE 64
#define fatal_err(...) \
	do { \
		char buff[BUFF_SIZE]; \
		fprintf(stderr,"\n"); \
		fprintf(stderr, "FILE:%s\n", __FILE__); \
        fprintf(stderr, "FUNCTION:%s\n", __FUNCTION__); \
        fprintf(stderr,"LINE:%d\n",__LINE__); \
        snprintf(buff, sizeof(buff),__VA_ARGS__); \
		fatal_err_callee(buff); \
	} while (0)

#ifdef DEBUG
#define print_info(...) \
    do { \
        char info_buff[INFO_BUFF_SIZE]={'\0'}; \
        snprintf(info_buff, sizeof(info_buff), __VA_ARGS__); \
        print_info_callee(info_buff); \
    } while(0)

#else
#define print_info(...) \
    { \
        \
    } while(0);
#endif // DEBUG


const static uint16_t MAX_HASH=(GRID_SIZE*GRID_SIZE);

enum SINGLE_VALUE // values of cells
{
    ONE = 1,
    // ...
    NINE = 1<<(GRID_SIZE-1)//1<<8

};

struct sudoku_state
{
    uint16_t min_hash;
    uint16_t hash; // sum of cells with 0 contradiction
    uint16_t cell[GRID_SIZE][GRID_SIZE];
    bool cell_isv[GRID_SIZE][GRID_SIZE]; // cell is single valued

    uint8_t contradiction[GRID_SIZE][GRID_SIZE];

    // Padding if required

};
typedef struct sudoku_state ss_t;

uint16_t all_possible_in_cell(void);
uint16_t compute_hash(ss_t);

bool is_single_value(uint16_t );
bool sudoku_states_equal(ss_t, ss_t);
bool sudoku_is_solved_hashwise(ss_t);
bool sudoku_is_solved_cellwise(ss_t ss);

void init_isv(ss_t * const ss_ptr);

ss_t get_ss(char const * const ip, char const * const op);

void fprint_ss(FILE *of, ss_t ss);
unsigned count_zero_bits_on_right(uint32_t);

bool ro_co_re(uint8_t,uint8_t,uint8_t,uint8_t);
bool in_region(uint8_t,uint8_t,uint8_t,uint8_t );
bool in_range(uint8_t v,uint8_t min, uint8_t max);


noreturn void solve
(char const * const ip, char const * const op,double const rt, unsigned nmax); // help from cboard forum
void check_time(time_t time_beg,double rt_input);
void update_3
(ss_t *const root_ptr, ss_t *const ss1_ptr,ss_t *const ss2_ptr);
void end_ptr_is_null
(ss_t *const root_ptr,ss_t *const ss1_ptr, ss_t *const ss2_ptr,
 char const *const ip, char const* const op, FILE* of);
void solve_info(int row, int column,size_t n1,size_t n2);

bool is_unsolvable (ss_t);
uint16_t eliminate_single_value(uint16_t,uint16_t);

void implement_constraints(ss_t *);

void update_isv_hash(ss_t * const ss_ptr);

void compute_contradictions(ss_t *);


ss_t *
fill_cells(ss_t *, size_t,bool, char const * const op, unsigned nmax);// help from cboard forum
ss_t * go_back(ss_t *root_ptr,ss_t * end_ptr);
void print_cellwise_solved(ss_t const *const end_ptr, char const *const op, size_t n);
void print_contra (ss_t const *const end_ptr, bool contra, bool pre_contra );
uint16_t first_multi_value_cell(ss_t const*const current_ptr, int *row_ptr, int *column_ptr);
void print_4_cells
(uint16_t root_cell, uint16_t current_cell,uint16_t cell, uint16_t end_cell);


uint16_t next_cell(uint16_t cell);
uint8_t try_next(uint16_t cell);

void update(ss_t * const, ss_t * const);

void previous_cell(int *const row, int *const column);
bool has_contra (ss_t ss);
bool def_no_solution(ss_t ss);

noreturn void fatal_err_callee(const char * const); // help from comp.lang.c
void print_info_callee(const char * const s);

void *my_alloc(size_t );

uint8_t popcnt(uint32_t v);
void first_uneq_cell
(const ss_t current_ptr,const ss_t end_ptr,int *const row,int *const column);

bool is_repeated
(ss_t *const root_ptr,ss_t *const end_ptr);

void print_finish(char const * const ip, char const * const op);

void print_cell_callee(const char* const cell_name,uint16_t cell);
#define print_cell(var_name) print_cell_callee(#var_name,var_name)

noreturn void print_version(void);
noreturn void print_help(void);
char* get_arg(char const*const arg_str, char *beg_str);

int main(int argc, char *argv[])
{
    int i;
    char *i_ptr;
    char *o_ptr;

    char inputf_name[LINE_LEN]="sudoku_input.text";

    char outputf_name[LINE_LEN]="sudoku_output.text";
    double rt_input=RUN_TIME;
    char *t_ptr;
    unsigned nmax=N_MAX;
    char *n_ptr;

    for(i=1; i<argc; i++)
    {
        if(strcmp(argv[i], "-h")==0)
            print_help();
        else if(strcmp(argv[i], "-v")==0)
            print_version();

        i_ptr=get_arg(argv[i], "-inputf=");
        if(i_ptr!=NULL)
            strncpy(inputf_name, i_ptr, LINE_LEN-1);

        o_ptr=get_arg(argv[i], "-outputf=");
        if(o_ptr!=NULL)
            strncpy(outputf_name, o_ptr, LINE_LEN-1);

        t_ptr=get_arg(argv[i], "-time=");
        if(t_ptr!=NULL)
        {
            printf("time is %f\n",rt_input);
            rt_input=atof(t_ptr);
            printf("time is %f\n",rt_input);
        }

        n_ptr=get_arg(argv[i], "-nmax=");
        if(n_ptr!=NULL)
        {
            printf("nmax is %u\n",nmax);
            nmax=(unsigned)strtoul(n_ptr, NULL, 10);
            printf("nmax is %u\n",nmax);
        }

    }

    solve(inputf_name, outputf_name, rt_input, nmax);

    return EXIT_SUCCESS;
}

char* get_arg(char const*const arg_str, char *beg_str) // TODO default_str can be input to simplify main()
{
    char *i_ptr;
    char *out_str=NULL;

    i_ptr=strstr(arg_str,beg_str);
    if(i_ptr!=NULL)
    {
        out_str=strchr(arg_str, '=');
        if(out_str==NULL)
            fatal_err("Wrong argument, arg is %s beg is %s",arg_str, beg_str);
    }
    else // i_ptr==NULL
        return NULL;

    printf("str is %s\n",out_str+1);
    return (out_str+1);
}

noreturn void print_version(void)
{
    printf("Version is %s\n",VERSION);
    exit (EXIT_SUCCESS);
}
noreturn void print_help(void)
{
    printf("Enter -h as argument to see this help.\n");
    printf("Enter -v to display version.\n");
    printf("Enter -inputf=NAME.EXT to override default values.\n");
    printf("Enter -outputf=NAME.EXT to override default values.\n");
    printf("Enter -time=SECONDS to set the rough max execution time.\n");
    printf("Enter -nmax=NUM to set the max states for each Sudoku puzzle.");
    exit (EXIT_SUCCESS);
}
//////////////////////////////////////////////

void check_time(time_t time_beg, double rt)
{
    time_t time_end;
    time(&time_end);
    double dt = difftime( time_end, time_beg );

    if(dt >= rt)
        fatal_err("time is over, time passed %f seconds",dt);
}

void update_3
(ss_t *const root_ptr, ss_t *const ss1_ptr,ss_t *const ss2_ptr)
{

    implement_constraints(ss1_ptr);
    *ss2_ptr=*ss1_ptr;

    update_isv_hash(ss2_ptr);

    compute_contradictions(ss2_ptr);

    if(is_unsolvable(*ss2_ptr))
        fatal_err("Problem is unsolvable after implementing constraints.");


    *root_ptr=*ss2_ptr;

}

void end_ptr_is_null
(ss_t *const root_ptr,ss_t *const ss1_ptr, ss_t *const ss2_ptr,
 char const * const ip, char const *const op, FILE* of)
{
    *root_ptr=get_ss(ip, op);
    *ss2_ptr=*root_ptr;
    *ss1_ptr=*ss2_ptr;
    fprintf(of, "end_ptr is NULL:\n");
    fprint_ss(of, *ss2_ptr);
}

void solve_info(int row, int column,size_t n1,size_t n2)
{
    print_info("// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //\n");

    print_info("row is %d, column is %d, n is %zu n2 is %zu\n"
               ,row,column,n1,n2);

    print_info("// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //\n");
}

noreturn void solve
(char const * const ip, char const * const op, double const rt_input, unsigned nmax)
{
    time_t time_beg;
    time(&time_beg);

    ss_t ss;
    ss_t ss2;

    ss_t *root_ptr;
    ss_t *end_ptr;
    size_t n=1;
    size_t n2=0;
    int row=0;
    int column=0;

    long pos=0;// file positioning

    FILE* of=fopen(op, "wt");
    if(of==NULL)
        fatal_err("Initially can not open output file %s", op);
    fprintf(of, "%c\n", output_sep);
    fclose(of);

    FILE* sif=NULL;
    do
    {
        check_time(time_beg, rt_input);

        of=fopen(op, "at");
        if(of==NULL)
            fatal_err("Can not open output file %s", op);

        sif=fopen(ip, "rt");
        if(sif==NULL)
            fatal_err("failed to open %s",ip);
        fseek(sif, pos * (long)sizeof(char), SEEK_SET);
        if(feof(sif) )
        {
            print_finish(ip, op);
        }

        long pre_pos7;
        pre_pos7=pos;
        fseek(sif, pos * (long)sizeof(char), SEEK_SET);
        pos=ftell(sif);

        root_ptr=my_alloc(PRE_ALLOC*sizeof (ss_t) );
        if(root_ptr==NULL)
            fatal_err("root_ptr malloc failed.");
        end_ptr=root_ptr+1;

        ss=get_ss(ip, op);

        if(is_unsolvable(ss))
            fatal_err("Problem is unsolvable initially.");
        if(has_contra(ss))
            fatal_err("Problem has contradiction initially.");

        update_3(root_ptr,&ss,&ss2);


        print_info("// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //\n");
        if(sudoku_is_solved_cellwise(ss2))
        {
            printf("Problem is solved after implementing constraints.\n");

            fprint_ss(stdout, ss2);

            continue;
        }

        ss_t * temp_ptr=realloc(root_ptr,(n+PRE_ALLOC)*sizeof (ss_t)); // 0..n-1
        if(temp_ptr!=NULL)
            root_ptr=temp_ptr;
        else
            fatal_err("realloc failed");

        end_ptr=fill_cells(root_ptr,n,false, op,nmax);
        if(end_ptr==NULL)
        {
            end_ptr_is_null( root_ptr,&ss,&ss2,ip,op,of);

            continue;
        }

        if(end_ptr->hash==MAX_HASH)
            printf("Filled all cells,row is %d column is %d n is %zu\n",row,column,n);

        if(sudoku_is_solved_cellwise(*end_ptr))
        {
            printf("Problem is solved after filling.\n");

            continue;
        }


        solve_info(row, column, n, n2);

        update_isv_hash(&ss2);
        compute_contradictions(end_ptr);

        fprint_ss(stdout, *end_ptr);

        if(root_ptr!=NULL)
            free(root_ptr);
        if(end_ptr!=NULL)
            free(end_ptr);

        fclose(sif);
        fclose(of);
    }
    while(true);
}

uint16_t next_cell(uint16_t cell)
{
    uint8_t next=try_next(cell);
    uint16_t first_cell=(uint16_t)(1<<(next-1));

    if(!is_single_value(first_cell))
        fatal_err("first_cell is %u",first_cell);

    print_info("^ %s:cell is %u first_cell is %u\n"
               ,__FUNCTION__,cell,first_cell);

    return first_cell;
}

uint8_t try_next(uint16_t cell) // TODO order can be random rather than 1..9
{
    unsigned count;
    uint8_t next=0;

    if(cell==0)
        fatal_err("Initial cell is %u",cell);

    count=count_zero_bits_on_right(cell);

    next=(uint8_t)(count+1);

    if(!(next>=1 && next<=9))
        fatal_err("Invalid next %u",next);

    return next;
}

///////////////////////////////////////
ss_t * go_back(ss_t *root_ptr,ss_t * end_ptr)
{
    ss_t * curr1_ptr;
    for(curr1_ptr=end_ptr-1; curr1_ptr>root_ptr; curr1_ptr--)
    {
        if(!def_no_solution(*curr1_ptr))
        {
            implement_constraints((curr1_ptr));
            *(end_ptr)=*(curr1_ptr);

            if(!def_no_solution(*end_ptr))
                break;
        }

    }
    return curr1_ptr;
}

void print_cellwise_solved(ss_t const *const end_ptr, char const *const op, size_t n)
{
    print_info("Problem is solved, n is %zu\n",n);

    fprint_ss(stdout, *end_ptr);

    FILE* of=fopen(op, "at");
    fprint_ss(of, *end_ptr);
    fprintf(of, "%c\n", output_sep);

    fclose(of);
}

void print_contra (ss_t const *const end_ptr,bool contra, bool pre_contra )
{

    contra=has_contra(*end_ptr);

    char pc_str[10]= {'\0'};
    char c_str[10]= {'\0'};
    pre_contra==true ? (strncpy(pc_str,"true",8)):(strncpy(pc_str,"false",8));
    contra==true ? (strncpy(c_str,"true",8)):(strncpy(c_str,"false",8));
    print_info("$ pre_contra is %s, contra is %s \n",pc_str,c_str);

    return;
}

uint16_t first_multi_value_cell(ss_t const*const current_ptr, int *row_ptr, int *column_ptr)
{
    uint16_t current_cell=current_ptr->cell[0][0];
    int row=*row_ptr;
    int column=*column_ptr;

    column=0;
    for(row=0; row<=(GRID_SIZE-1); row++)
    {
        current_cell=current_ptr->cell[row][column];
        if(!is_single_value(current_cell))
            break;
        for(column=0; column<=(GRID_SIZE-1); column++)
        {

            current_cell=current_ptr->cell[row][column];
            if(!is_single_value(current_cell))
            {
                if(current_cell==0)
                    fatal_err("` current_cell is 0, row is %d, column is %d\n",row,column);

                print_info("` contra is false, row is %d, column is %d \n",row,column);
                print_cell(current_cell);

                break;
            }
        }

        if(column>(GRID_SIZE-1))
        {
            column=0;

            continue;
        }

        current_cell=current_ptr->cell[row][column];
        if(!is_single_value(current_cell))
            break;
    }
    if(row>(GRID_SIZE-1))
    {
        row=GRID_SIZE-1;
        column=GRID_SIZE-1;
    }

    *row_ptr=row;
    *column_ptr=column;

    return current_cell;
}

void print_4_cells
(uint16_t root_cell, uint16_t current_cell,uint16_t cell, uint16_t end_cell)
{
    print_cell(root_cell);
    print_cell(current_cell);
    print_cell(cell);
    print_cell(end_cell);

    return;
}


ss_t * fill_cells
( ss_t * root_ptr, size_t n, bool pre_contra, char const * const op, unsigned nmax)
{
    uint16_t root_cell=0;
    uint16_t end_cell=0;

    int row=0;
    int column=0;
    static ss_t * end_ptr;
    bool contra=false;
    static ss_t  *current_ptr; // ?

    if(root_ptr==NULL)
        fatal_err("wrong pointer.");


    ss_t* temp_ptr=realloc(root_ptr,(n+PRE_ALLOC)*sizeof (ss_t));
    if(temp_ptr!=NULL)
        root_ptr=temp_ptr;
    else
        fatal_err("root_ptr realloc failed. n is %zu",n);

    if(n==0)
        fatal_err("n is 0");
    if(n>=nmax )
        fatal_err("n exceeded nmax, n is %zu",n);

    print_info("\n--------------------------------\n");
    print_info("$$ Beggining %s function.\n",__FUNCTION__);

    end_ptr=root_ptr+n;
    ss_t * last_ptr=(end_ptr+1);
    last_ptr=NULL;
    last_ptr++;
    last_ptr=NULL;

    ss_t * curr1_ptr;
    curr1_ptr=go_back(root_ptr, end_ptr);

    ptrdiff_t diff1=end_ptr-curr1_ptr;
    print_info("$ end_ptr-curr1_ptr (ptrdiff) is %td\n",diff1);

    update(root_ptr,curr1_ptr);
    *(end_ptr)=*(curr1_ptr);

    if(sudoku_is_solved_cellwise(*end_ptr))
    {
        print_cellwise_solved(end_ptr,op,n);

        return NULL;
    }

    print_contra(end_ptr, contra, pre_contra);

    current_ptr=curr1_ptr;

    ptrdiff_t diff=end_ptr-current_ptr;

    print_info("$ end_ptr-current_ptr (ptrdiff) is %td\n",diff);

    print_info("$ n is %zu\n",n);

    row=0;
    column=0;
    root_cell=root_ptr->cell[row][column];
    end_cell=end_ptr->cell[row][column];

    update(root_ptr,current_ptr);
    fprint_ss(stdout, *(current_ptr));

    first_uneq_cell(*current_ptr,*end_ptr,&row,&column);

    print_info("`` first_uneq_cell, row is %d column is %d\n",row,column);
    uint16_t cc1=all_possible_in_cell();
    uint16_t ec1=all_possible_in_cell();
    if(!(row==GRID_SIZE-1 && column==GRID_SIZE-1))
    {
        print_info("$ row is %d, column is %d\n",row,column);
        cc1=current_ptr->cell[row][column];
        ec1=end_ptr->cell[row][column];
        print_cell(cc1);
        print_cell(ec1);

        row=0;
        column=0;
    }

    bool moving_forward= true;

    if(is_single_value(cc1)&&is_single_value(ec1)&& ec1<cc1)
    {
        print_info("$ moving_forward is false\n");

        moving_forward=false;
    }

    if(!moving_forward)
        fatal_err("!moving_forward");

    if(moving_forward)
    {
        print_info("` Moving forward.\n");

        uint16_t current_cell;

        current_cell=first_multi_value_cell(current_ptr,&row, &column);

        print_info("` row is %d, column is %d\n",row,column);
        root_cell=root_ptr->cell[row][column];
        current_cell=current_ptr->cell[row][column];
        uint16_t cell=current_ptr->cell[row][column];

        print_4_cells(root_cell,current_cell, cell, end_cell);

        uint16_t n_cell=next_cell(cell);
        uint16_t esv_cell=eliminate_single_value(n_cell,cell);

        print_cell(n_cell);
        print_cell(esv_cell);

        *end_ptr=*current_ptr;
        end_ptr->cell[row][column]=n_cell;

        init_isv(end_ptr);

        update(root_ptr,end_ptr);

        bool ir=is_repeated(root_ptr,end_ptr);

        uint8_t pc=popcnt(n_cell);
        if(ir)
            print_info("`` ir is true, pc is %u\n",pc);

        end_ptr->cell[row][column]=n_cell;

        update(root_ptr,end_ptr);

        print_info("`` contra is false, row is %d, column is %d \n",row,column);
        print_cell(cell);

        if(cell==0)
            fatal_err("cell is %u",cell);

        init_isv(current_ptr);

        current_ptr->cell[row][column]=esv_cell;

        update(root_ptr,current_ptr);

        print_cell(esv_cell);

        bool hce=has_contra(*end_ptr);

        fill_cells(root_ptr,n+1,hce, op, nmax); // recursion
    }

    return end_ptr;
}

void init_isv(ss_t * const ss_ptr)
{
    unsigned row;
    unsigned column;

    ss_ptr->min_hash=0;
    for(row=0; row<=GRID_SIZE-1; row++)
        for(column=0; column<=GRID_SIZE-1; column++)

            ss_ptr->cell_isv[row][column]=false;

    return;
}


void update_isv_hash(ss_t *const ss_ptr) // after implement constraints
{
    unsigned row;
    unsigned column;

    uint16_t hash=0;

    init_isv(ss_ptr);
    for(row=0; row<=GRID_SIZE-1; row++)
        for(column=0; column<=GRID_SIZE-1; column++)
            if(is_single_value(ss_ptr->cell[row][column]))
                ss_ptr->cell_isv[row][column]=true;

    hash=compute_hash(*ss_ptr);

    ss_ptr->hash=hash;

    return ;

}

uint16_t all_possible_in_cell(void)
{
    uint16_t r=0;
    enum SINGLE_VALUE sv;
    for(sv=ONE; sv<=NINE; sv<<=1)
        r |= sv;

    return r;
}

uint16_t compute_hash(ss_t ss)
{
    uint16_t hash=0;

    unsigned row;
    unsigned column;

    for(row=0; row<=GRID_SIZE-1; row++)
        for(column=0; column<=GRID_SIZE-1; column++)
            if(ss.cell_isv[row][column])
            {
                hash++;

            }


    if(hash<ss.min_hash || hash>MAX_HASH)
    {
        fatal_err("wrong hash in compute_hash, hash is %u",hash);
    }

    return hash;
}

unsigned count_zero_bits_on_right(uint32_t v)
{
    // https://graphics.stanford.edu/~seander/bithacks.html#ZerosOnRightBinSearch
    unsigned c;

    if(v==0)
        fatal_err("failed, v is %u",v);

    if (v & 0x1)
    {
        c = 0;
    }
    else
    {
        c = 1;
        if ((v & 0xffff) == 0)
        {
            v >>= 16;
            c += 16;
        }
        if ((v & 0xff) == 0)
        {
            v >>= 8;
            c += 8;
        }
        if ((v & 0xf) == 0)
        {
            v >>= 4;
            c += 4;
        }
        if ((v & 0x3) == 0)
        {
            v >>= 2;
            c += 2;
        }
        c -= v & 0x1;
    }

    if(c>GRID_SIZE) // was if(c>=9)
        fatal_err("Count zero bits failed, c is %u\n",c);

    return c;
}


bool is_single_value(uint16_t value)
{
    bool b=false;
    enum SINGLE_VALUE sv;

    for(sv=ONE; sv<=NINE; sv<<=1)
        if(value == sv)
            b=true;

    return b;

}

bool sudoku_states_equal(ss_t ss1, ss_t ss2)
{
    unsigned row;
    unsigned column;
    bool r=true;
    uint16_t ss1_rc;
    uint16_t ss2_rc;

    if(ss1.hash != ss2.hash)
        r=false;

    if(r==true)
    {
        for(row=0; row<=GRID_SIZE-1; row++)
            for(column=0; column<=GRID_SIZE-1; column++)
            {
                ss1_rc=ss1.cell[row][column];
                ss2_rc=ss2.cell[row][column];

                if(ss1_rc!=ss2_rc)
                    r=false;
            }
    }


    if(r==true && (ss1.hash!=ss2.hash) )
        fatal_err("sudoku states are equal, but hashes are not.");

    return r;
}

bool sudoku_is_solved_hashwise(ss_t ss)
{
    bool r=false;
    if(ss.hash == MAX_HASH)
        r=true;

    return r;
}

bool sudoku_is_solved_cellwise(ss_t ss)
{
    unsigned row;
    unsigned column;
    bool r=true;
    uint8_t contradiction;
    bool isv;

    for(row=0; row<=GRID_SIZE-1; row++)
        for(column=0; column<=GRID_SIZE-1; column++)
        {
            contradiction=ss.contradiction[row][column];
            isv=is_single_value(ss.cell[row][column]);

            if(contradiction!=0 || !isv)
                r=false;
        }


    return r;
}


uint16_t eliminate_single_value(uint16_t remove_this,uint16_t original)
{
    uint16_t sc3= original&(~remove_this);
    return sc3;
}

// TODO this function could be called several times untill hash stayes the same
void implement_constraints(ss_t * const ss_ptr)
{
    uint8_t row;
    uint8_t column;
    uint8_t row2;
    uint8_t column2;
    uint16_t sc1=all_possible_in_cell();
    uint16_t sc2=all_possible_in_cell();
    uint16_t hash=0;

    for(row=0; row<=GRID_SIZE-1; row++)
        for(column=0; column<=GRID_SIZE-1; column++)
            for(row2=0; row2<=GRID_SIZE-1; row2++)
                for(column2=0; column2<=GRID_SIZE-1; column2++)
                {
                    if(row==row2 && column==column2)
                        continue;
                    if(ro_co_re(row,column,row2,column2))
                    {
                        sc1=ss_ptr->cell[row][column];
                        sc2=ss_ptr->cell[row2][column2];
                        if(is_single_value(sc1) && !is_single_value(sc2) )
                            sc2=eliminate_single_value(sc1,sc2);

                        if(!is_single_value(sc1) && is_single_value(sc2) )
                            sc1=eliminate_single_value(sc2,sc1);
                        ss_ptr->cell[row][column]=sc1;
                        ss_ptr->cell[row2][column2]=sc2;
                    }
                }



    hash=compute_hash(*ss_ptr);

    if(hash<ss_ptr->min_hash || hash>MAX_HASH)
        fatal_err("wrong hash, hash is %u",hash);
    ss_ptr->hash=hash;

    compute_contradictions(ss_ptr);

    return;
}

void compute_contradictions(ss_t *ss)
{
    uint8_t row;
    uint8_t column;
    uint8_t row2;
    uint8_t column2;
    bool rcr=false;
    uint16_t cell_1;
    uint16_t cell_2;
    bool cell_1_isv;
    bool cell_2_isv;

    if(ss==NULL)
        fatal_err("wrong pointer.");

    for(row=0; row<=GRID_SIZE-1; row++)
        for(column=0; column<=GRID_SIZE-1; column++)
            ss->contradiction[row][column]=0;

    for(row=0; row<=GRID_SIZE-1; row++)
        for(column=0; column<=GRID_SIZE-1; column++)
            for(row2=0; row2<=GRID_SIZE-1; row2++)
                for(column2=0; column2<=GRID_SIZE-1; column2++)
                {
                    if(row==row2 && column==column2)
                        continue;
                    rcr=ro_co_re(row,column,row2,column2);
                    cell_1=ss->cell[row][column];
                    cell_2=ss->cell[row2][column2];
                    cell_1_isv = is_single_value(cell_1);
                    cell_2_isv=is_single_value(cell_2);
                    if( (rcr || row==row2||column==column2)&& cell_1==cell_2 && cell_1_isv &&cell_2_isv)
                    {
                        ss->contradiction[row][column]++;

                    }


                }
    return;
}

bool ro_co_re(uint8_t ro1,uint8_t co1,uint8_t ro2,uint8_t co2) // row column region
{
    bool r=false;

    if(ro1==ro2 || co1==co2)
        r=true;

    if(in_region(ro1,co1,ro2,co2))
        r=true;

    return r;
}

bool in_region(uint8_t ro1,uint8_t co1,uint8_t ro2,uint8_t co2)
{
    bool r=false;

    if(in_range(ro1,0,2) && in_range(co1,0,2) && in_range(ro2,0,2) && in_range(co2,0,2))
        r=true;
    if(in_range(ro1,0,2) && in_range(co1,3,5) && in_range(ro2,0,2) && in_range(co2,3,5))
        r=true;
    if(in_range(ro1,0,2) && in_range(co1,6,8) && in_range(ro2,0,2) && in_range(co2,6,8))
        r=true;
    if(in_range(ro1,3,5) && in_range(co1,0,2) && in_range(ro2,3,5) && in_range(co2,0,2))
        r=true;
    if(in_range(ro1,3,5) && in_range(co1,3,5) && in_range(ro2,3,5) && in_range(co2,3,5))
        r=true;
    if(in_range(ro1,3,5) && in_range(co1,6,8) && in_range(ro2,3,5) && in_range(co2,6,8))
        r=true;
    if(in_range(ro1,6,8) && in_range(co1,0,2) && in_range(ro2,6,8) && in_range(co2,0,2))
        r=true;
    if(in_range(ro1,6,8) && in_range(co1,3,5) && in_range(ro2,6,8) && in_range(co2,3,5))
        r=true;
    if(in_range(ro1,6,8) && in_range(co1,6,8) && in_range(ro2,6,8) && in_range(co2,6,8))
        r=true;

    return r;
}

bool in_range(uint8_t v,uint8_t min, uint8_t max )
{
    bool r=false;

    if(v>=min && v<=max)
        r=true;

    return r;
}

bool is_unsolvable (ss_t ss)
{
    uint8_t row;
    uint8_t column;
    bool r=false;
    uint16_t cell=0;
    uint16_t apic=all_possible_in_cell();

    for(row=0; row<=GRID_SIZE-1; row++)
    {
        for(column=0; column<=GRID_SIZE-1; column++)
        {
            cell=ss.cell[row][column];
            apic=all_possible_in_cell();
            if(cell==0 || cell>apic)
                r=true;
        }
        if(column>GRID_SIZE-1)
            column=GRID_SIZE-1;
        cell=ss.cell[row][column];
        apic=all_possible_in_cell();
        if(cell==0 || cell>apic)
            r=true;
    }

    return r;
}

bool has_contra (ss_t ss)
{
    uint8_t row=0;
    uint8_t column=0;
    bool r=false;

    for(row=0; row<=GRID_SIZE-1; row++)
        for(column=0; column<=GRID_SIZE-1; column++)
            if(ss.contradiction[row][column]!=0)
            {
                print_info("$$ $ %s: row is %u column is %u\n ",__FUNCTION__,row,column);
                print_info("$$ $$ ");
                print_cell(ss.cell[row][column]);
                return true;
            }

    return r;
}

bool def_no_solution(ss_t ss)
{
    bool r;
    r= is_unsolvable(ss) || has_contra(ss);
    return r;
}

///////////////////////////////////////////////////
void update(ss_t * const root_ptr,ss_t * const ss_ptr)
{

    if(root_ptr==NULL || ss_ptr==NULL )
        fatal_err("wrong pointer.");

    implement_constraints((ss_ptr));

    update_isv_hash(ss_ptr);
    compute_contradictions(ss_ptr);

    ss_ptr->min_hash=root_ptr->hash;

    return;

}

void previous_cell(int *const row, int *const column)
{
    if(row==NULL || column==NULL)
        fatal_err("wrong pointer.");

    if(*row==0 && *column==0)
        fatal_err("Can not step back any further.");

    if(*column==0)
    {
        (*row)--;
        *column=GRID_SIZE-1;

    }
    else
        (*column)--;

    return;
}


ss_t get_ss(char const * const ip, char const * const op)
{
    unsigned row=0;
    unsigned column=0;

    ss_t ss1
    = {.min_hash=0,.hash=0,.cell={{0}},.cell_isv={{false}},.contradiction={{0}} };
    FILE *f_input = NULL;
    char line[LINE_LEN]= {'\n','\0'};
    char ch='0';// int for EOF

    uint16_t apic=0;
    uint16_t hash=0;
    ss_t *ss_ptr=&ss1;

    static long pos=0;

    if(ss_ptr==NULL)
        fatal_err("wrong pointer.");

    f_input=fopen(ip,"rt");
    if(f_input==NULL)
        fatal_err("Opening input file failed.");

    if(pos==EOF)
        fatal_err("wrong pos, pos is EOF");

    if(pos==EOF)
        fatal_err("pos failed.");

    fseek(f_input, pos * (long)sizeof(char), SEEK_SET);

    print_info("``` up_pos is %ld, current_ch is %c\n",pos,getc(f_input));

    do
    {

        fscanf(f_input,"%125s\n", line);
        if(feof(f_input) )
        {
            print_finish(ip, op);
        }

    }
    while(strchr(line,input_sep)!=NULL);

    int oc=ungetc(line[0],f_input);
    if(oc==EOF)
        fatal_err("ungetc failed, EOF is returned.");

    for (row=0; row<=GRID_SIZE-1; row++)
    {

        for (column=0; column<=GRID_SIZE-1; column++)
        {
            do
                fscanf(f_input,"%c ",&ch);
            while(ch==' ' || ch=='\n')
                ;
            if( ! ((ch>='1' && ch<='9') || (ch=='*') ) )

                fatal_err("Unacceptable input char, char is %c",ch);

            apic=all_possible_in_cell();

            ss1.cell[row][column] = (uint16_t)(ch=='*' ? apic : 1<<(ch-'1') );
            ss1.cell_isv[row][column] = (ch=='*'? false: true);

        }

    }

    pos = ftell(f_input);

    if(pos==EOF)
        fatal_err("pos failed.");

    print_info("``` down_pos is %ld, current_ch is %c\n",pos,getc(f_input));

    fclose(f_input);

    hash=compute_hash(ss1);
    ss1.hash=hash;

    ss1.min_hash=hash;

    compute_contradictions(ss_ptr);

    return ss1;
}


void fprint_ss(FILE* of,ss_t ss)
{

    unsigned row;
    unsigned column;
    uint16_t sc=0;
    unsigned uc;

    if(of==stdout)
    {
#ifdef DEBUG
        fprintf(of,"\nhash is %u\n",ss.hash);


        fprintf(of,"Value in cells:\n");
        for(row=0; row<=GRID_SIZE-1; row++)
        {
            for(column=0; column<=GRID_SIZE-1; column++)
            {

                uc=count_zero_bits_on_right(ss.cell[row][column]);
                fprintf(of,"%u ",uc+1);
            }
            fprintf(of,"\n");
        }
#endif // DEBUG
    }
    else
    {
        if(of !=stdout)
            ftell(of);
    }

    if(of != stdout)
    {
        for(row=0; row<=GRID_SIZE-1; row++)
        {
            for(column=0; column<=GRID_SIZE-1; column++)
            {

                sc=ss.cell[row][column];
                if(is_single_value(sc))
                {

                    fprintf(of,"%u ",count_zero_bits_on_right(sc)+1);
                    if(of !=stdout)
                        ftell(of);
                }
                else
                {

                    fprintf(of,"%c ",'*');
                    if(of !=stdout)
                        ftell(of);
                }
            }

            fprintf(of,"\n");
            if(of !=stdout)
                ftell(of);
        }
    }

    if(of==stdout)
    {
#ifdef DEBUG
        //printf("\n");
        fprintf(of,"\nBounded cells:\n");
        for(row=0; row<=GRID_SIZE-1; row++)
        {
            for(column=0; column<=GRID_SIZE-1; column++)
                //printf(ibc.cell[row][column]==true?"1 ":"0 ");
                fprintf(of,ss.cell_isv[row][column]==true?"1 ":"0 ");
            fprintf(of,"\n");
        }

        fprintf(of,"\nContradictions:\n");
        for(row=0; row<=GRID_SIZE-1; row++)
        {
            for(column=0; column<=GRID_SIZE-1; column++)
                fprintf(of,"%d ",ss.contradiction[row][column]);
            fprintf(of,"\n");
        }
#endif // DEBUG
    }


}


noreturn void fatal_err_callee(const char * const s) // help from comp.lang.c
{
    if ((s == NULL) || (*s == '\0'))
    {
        fprintf(stderr, "Please provide a proper message for "
                "fatal_err_callee function.\n");
        exit(EXIT_FAILURE);
    }
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdate-time"

    fprintf(stderr,"DATE:%s\n",__DATE__);
    fprintf(stderr,"TIME:%s\n",__TIME__);

#pragma clang diagnostic pop

    fprintf(stderr, "\nERROR:%s\n", s);
    exit(EXIT_FAILURE);
}

void print_info_callee(const char * const s)
{
    if ((s == NULL) || (*s == '\0'))
    {
        fprintf(stderr, "Please provide a proper message for "
                "print_info_callee function.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout,"%s\n",s);

    return;
}

void *my_alloc( size_t size )
{
    void *alloc_ptr = NULL; // see: http://c-faq.com/ptrs/passptrinit.html
    alloc_ptr = malloc(size);

    if( alloc_ptr == NULL )
        fatal_err("alloc_ptr memory allocation failed.");

    return alloc_ptr;
}

uint8_t popcnt(uint32_t v)
{
    //https://graphics.stanford.edu/~seander/bithacks.html
    uint8_t c;
    for (c = 0; v; c++)
    {
        v &= v - 1; // clear the least significant bit set
    }
    return c;
}

void first_uneq_cell
(const ss_t current_ptr, const ss_t end_ptr,int *const row,int *const column)
{
    uint16_t current_cell=0;
    uint16_t end_cell=0;

    if( *row<0 || *column<0)
        fatal_err("Wrong sub.");

    *row=0;
    *column=0;

    for(*row=0; *row<=GRID_SIZE-1; (*row)++)
    {
        for(*column=0; *column<=GRID_SIZE-1; (*column)++)
        {
            current_cell=current_ptr.cell[*row][*column];
            end_cell=end_ptr.cell[*row][*column];
            if(current_cell!=end_cell )

                break;
        }
        if(*column>GRID_SIZE-1)
        {
            continue;
        }

        current_cell=current_ptr.cell[*row][*column];
        end_cell=end_ptr.cell[*row][*column];
        if(current_cell!=end_cell )

            break;
    }

    if(*row>GRID_SIZE-1)
    {
        *row=GRID_SIZE-1;
        *column=GRID_SIZE-1;
    }
    return;
}

bool is_repeated
(ss_t *const root_ptr,ss_t *const end_ptr)
{
    bool ir=false;
    ss_t *current_ptr=root_ptr;

    if(root_ptr>end_ptr)
        fatal_err("Wrong pointer.");

    for(; current_ptr<end_ptr; current_ptr++)
    {
        if(sudoku_states_equal(*current_ptr,*end_ptr))
        {
            ptrdiff_t pd=end_ptr-current_ptr;
            print_info("$ is_repeated diff %td\n",pd);
            ir=true;
            break;
        }

    }

    return ir;
}

noreturn void print_finish(char const * const ip, char const * const op)
{
    printf("EOF in input file %s reached.\n",ip);
    printf("Output file name is %s\n",op);
    exit (EXIT_SUCCESS);
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

void print_cell_callee(const char* const cell_name, uint16_t cell)
{
#ifdef DEBUG

    enum SINGLE_VALUE sv;
    unsigned char ch='0';

    if(cell_name[0]=='\0')
        fatal_err("Wrong cell_name");

    printf("`` %s (1..9) is ",cell_name);
    for(sv=ONE; sv<=NINE; sv<<=1)
    {
        uint16_t cell_sv=cell&sv;
        if(cell_sv)
        {
            ch=try_next(cell_sv)+'0';
            if(ch<'0' || ch>'9')
                fatal_err("Illegal char.");
            printf("%c ",ch);
        }


    }

    if(cell==0)
        printf("0");
    printf("\n");
    printf("%c",'\0');


#endif // DEBUG
}
#pragma clang diagnostic pop

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
