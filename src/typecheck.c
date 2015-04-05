#include "typecheck.h"
#include "parser.h"

struct type* char_to_type(char* ident_type)
{
    struct type* t = malloc(sizeof( struct type));
    if(strcmp(ident_type, "entier") == 0)
    {
        t->type_kind = primary_t;
        t->type_val.primary  = int_t;
    }
    else if(strcmp(ident_type, "reel") == 0)
    {
        t->type_kind = primary_t;
        t->type_val.primary = real_t;
    }
    else if(strcmp(ident_type, "boolen") == 0)
    {
        t->type_kind = primary_t;
        t->type_val.primary = bool_t;
    }
    else if(strcmp(ident_type, "caractere") == 0)
    {
        t->type_kind = primary_t;
        t->type_val.primary = char_t;
    }
    else if(strcmp(ident_type, "chaine") == 0)
    {
        t->type_kind = primary_t;
        t->type_val.primary = str_t;
    }

    return t;

}

bool equal_types(struct type* t1, struct type* t2)
{

    if(t1->type_kind != t2->type_kind)
        return false;

    switch(t1->type_kind)
    {
        case primary_t:
            printf("primary\n");
            return t1->type_val.primary == t2->type_val.primary; 
            break;
        case records_t:
            if(t1->type_val.records_type->fields->size != t2->type_val.records_type->fields->size)
                return false;
            for(unsigned i = 0; i < t1->type_val.records_type->fields->size; ++i)
            {
                struct field* field1 = list_nth(*(t1->type_val.records_type->fields), i);
                struct field* field2 = list_nth(*(t2->type_val.records_type->fields), i);

                if(!equal_types(field1->type, field2->type))
                    return false;
            }
            return true;
            break;
        case array_t:

            break;
        case pointer_t:
            break;
        case enum_t:
            break;

    }
    return true;
}

bool check_args(struct function* f, struct funcall call, 
        fun_table_t* functions, var_table_t* variables, type_table_t* types)
{
    if(f->arg.size != call.args.size)
        return false;
    for(unsigned i = 0; i < call.args.size; ++i)
    {
        if(!equal_types(get_expr_type((struct expr *)list_nth(call.args,i), functions, variables, types) 
                    ,list_nth(f->arg,i)->type))
            return false;
    }
    return true;
}

char *algo_to_c_type(char *ident)
{
    if (strcmp(ident, "entier") == 0)
        return "int";
    else if (strcmp(ident, "caractere") == 0)
        return "char";
    else if (strcmp(ident, "chaine") == 0)
        return "char *";
    else if (strcmp(ident, "reel") == 0)
        return "double";
    else if (strcmp(ident, "booleen") == 0)
        return "int";
    // If it is none of the primary types, then it is a user-defined type
    // wich will have the same name in the c file as in the algo file.
    return ident;
}

bool check_prog(struct prog* prog)
{
    printf("begin type check\n");
    fun_table_t* functions = empty_fun_table();
    for(unsigned i = 0; i < prog->algos.size; ++i)
    {
        struct algo* al = list_nth(prog->algos, i);
        struct function* f = malloc(sizeof(struct function));
        f->ident = al->ident;
        check_algo(al, functions);
    }
    free(functions);
    return true;
}

void add_variable(var_table_t* variables, type_table_t* types, struct single_var_decl* var)
{
    for(unsigned int j = 0; j < var->var_idents.size; ++j)
    {
        struct var_sym* sym = malloc(sizeof(struct var_sym));
        sym->ident = list_nth(var->var_idents, j);
        sym->type = find_type(types, var->type_ident)->type;
        add_var(variables, sym);
    }
}

bool check_algo(struct algo* al, fun_table_t* functions)
{
    struct function* f = malloc(sizeof(struct function));
    var_table_t* variables = empty_var_table();
    type_table_t* types = empty_type_table();
    struct declarations* decl = al->declarations;

    struct type* t_bool_ = malloc(sizeof(struct type));
    t_bool_->type_kind = primary_t;
    primary_type p1 = bool_t;
    t_bool_->type_val.primary = p1;
    struct type_sym* type_bool = malloc(sizeof(struct type));
    type_bool->ident = "booleen";
    type_bool->type = t_bool_;
    add_type(types,type_bool); 

    struct type* t_int_ = malloc(sizeof(struct type));
    t_int_->type_kind = primary_t;
    primary_type p2 = int_t;
    t_int_->type_val.primary = p2;
    struct type_sym* type_int = malloc(sizeof(struct type));
    type_int->ident = "entier";
    type_int->type = t_int_;
    add_type(types,type_int); 
    
    struct type* t_reel_ = malloc(sizeof(struct type));
    t_reel_->type_kind = primary_t;
    primary_type p3 = bool_t;
    t_reel_->type_val.primary = p3;
    struct type_sym* type_reel = malloc(sizeof(struct type));
    type_reel->ident = "reel";
    type_reel->type = t_reel_;
    add_type(types,type_reel); 
    
    struct type* t_char_ = malloc(sizeof(struct type));
    t_char_->type_kind = primary_t;
    primary_type p4 = char_t;
    t_char_->type_val.primary = p4;
    struct type_sym* type_char = malloc(sizeof(struct type));
    type_char->ident = "caractere";
    type_char->type = t_reel_;
    add_type(types,type_char); 
    
    if(decl != NULL)
    {
        struct param_decl* p_decl = decl->param_decl;
        struct local_param* loc = p_decl->local_param;
        struct global_param* glo = p_decl->global_param;
        vardecllist_t vars = decl->var_decl;
        typedecllist_t typelist = decl->type_decls;
        if(loc != NULL)
        {
            for(unsigned int i = 0; i < loc->param.size; i++)
            {
                struct single_var_decl* var = list_nth(loc->param,i);
                add_variable(variables, types, var);
            }
        }
        printf("loc done\n");
        if(glo != NULL)
        {
            for(unsigned int i = 0; i < glo->param.size; i++)
            {
                struct single_var_decl* var = list_nth(glo->param,i);
                add_variable(variables, types, var);
            }
        }
        printf("glob done\n");
        for(unsigned i = 0; i < typelist.size; ++i)
        {
            struct type_decl* type_decl = list_nth(typelist, i);
            struct type_def*  type_def  = type_decl->type_def;
            switch(type_def->type_type)
            {
                case enum_type:
                    {
                        printf("enum_type\n");
                        struct type* type = malloc(sizeof(struct type));
                        type->type_kind = enum_t;

                        struct enum_type* _enum = malloc(sizeof(struct enum_type));
                        _enum->idents = type_def->def.enum_def->identlist;

                        type->type_val.enum_type = _enum;

                        struct type_sym* sym = malloc(sizeof(struct type_sym));
                        sym->type = type;
                        sym->ident = type_decl->ident;

                        add_type(types, sym);
                    }
                    break;
                case array_type:
                    {
                        printf("array_type\n");
                        struct type* type = malloc(sizeof(struct type));
                        type->type_kind = array_t;

                        struct array* array = malloc(sizeof(struct array));
                        if((array->type = find_type(types, type_def->def.array_def->elt_type)->type) 
                                == NULL)
                        {
                            printf("unknown type\n");
                        }

                        array->dims = type_def->def.array_def->dims;

                        type->type_val.array_type = array;

                        struct type_sym* sym = malloc(sizeof(struct type_sym));
                        sym->type = type;
                        sym->ident = type_decl->ident;

                        add_type(types, sym);
                    }
                    break;
                case struct_type:
                    {
                        printf("struct_type\n");
                        struct type* type = malloc(sizeof(struct type));
                        type->type_kind = records_t;

                        struct records* record = malloc(sizeof(struct records));
                        fieldlist_t fields;
                        list_init(fields);

                        vardecllist_t varlist = type_def->def.record_def->var_decl;

                        for(unsigned int i = 0 ; i < varlist.size; ++i)
                        {
                            struct single_var_decl* var = list_nth(varlist, i);

                            for(unsigned int j = 0; j < var->var_idents.size; ++j)
                            {
                                struct field* field = malloc(sizeof(struct field));
                                field->ident = list_nth(var->var_idents,i);
                                field->type = find_type(types, var->type_ident)->type;

                                list_push_back(fields, field);
                            }
                        }

                        record->fields = &fields;

                        type->type_val.records_type = record;

                        struct type_sym* sym = malloc(sizeof(struct type_sym));
                        sym->type = type;
                        sym->ident = type_decl->ident;

                        add_type(types, sym);
                    }
                    break;
                case pointer_type:
                    break;

            }
        }
        for(unsigned i =0; i < vars.size; ++i)
        {
            struct single_var_decl* var = list_nth(vars, i);
            add_variable(variables, types, var);
        }
    }
    printf("treatement\n");
    for(unsigned i = 0; i < al->instructions.size; i++)
    {
        printf("new inst\n");
        if(!check_inst(list_nth(al->instructions, i), f, functions, variables, types)) 
        {
          //  free(f);
           // free_var_table(variables);
           // free_type_table(types);
            return false;
        }
    }
    free_var_table(variables);
    free_type_table(types);
    free(f);
    return true;
}

bool check_inst(struct instruction *i, struct function* f, fun_table_t* functions, var_table_t* variables, 
        type_table_t* types)
{
    switch(i->kind)
    {
        case funcall:
            {
                struct function* f = malloc(sizeof(struct function));
                f->ident = i->instr.funcall->fun_ident;
                if (get_function(functions, f->ident) != NULL)
                {
                    free(f);
                    return true;
                }
                else
                {
                    printf("implicit declaration of function %s",f->ident);
                    free(f);
                    return false;
                }
            }
            break;

        case assignment:

            printf("assignment\n");
            if(check_expr(i->instr.assignment->e1, functions, variables, types) 
                    && check_expr(i->instr.assignment->e2, functions, variables, types)){
                struct type* t1 = get_expr_type(i->instr.assignment->e1, functions, variables, types);
                struct type* t2 = get_expr_type(i->instr.assignment->e2, functions, variables, types); 
                if(!equal_types(t1,t2))
                {
                    printf("error in assignment\n");
                    free(t1);
                    free(t2);
                    return false;
                }
                free(t1);
                free(t2);
                return true;
            }
            else
            {
                return false;
            }
            break;

        case ifthenelse:
            {
                printf("ifthenelse\n");
                struct ifthenelse* e = i->instr.ifthenelse;

                struct type* t = malloc(sizeof(struct type));
                t->type_kind = primary_t;
                primary_type p = bool_t;
                t->type_val.primary = p;
                struct type* t1 = get_expr_type(e->cond, functions, variables, types);
                if(equal_types(t1, t))
                {
                    for(unsigned int i =0; i < e->instructions.size; ++i)
                    {
                        if (!check_inst(list_nth(e->instructions, i), f, functions, variables, types))
                        {
                            free(t1);
                            free(t);
                            return false;
                        }
                    }
                    for(unsigned int i =0; i < e->elseblock.size; ++i)
                    {
                        if(!check_inst(list_nth(e->elseblock,i), f, functions, variables, types))
                        {
                            free(t1);
                            free(t);
                            return false;
                        }
                    }
                    free(t1);
                    free(t);
                    return true;
                }

                printf("error in condition statement");
                free(t1);
                free(t);
                return false;
            }
            break;

        case switchcase:
            //TODO: implement switchcase
            break;

        case dowhile:
            {
                printf("dowhile\n");
                struct dowhile* e = i->instr.dowhile;
                if(equal_types(get_expr_type(e->cond, functions, variables, types), (struct type*) bool_t))
                {
                    for(unsigned int i = 0; i < e->instructions.size; ++i)
                    {
                        if(!check_inst(list_nth(e->instructions,i), f, functions, variables, types))
                        {
                            printf("error in instruction switch");
                            return false;
                        }
                    }
                    return true;
                }
                printf("error in condition of switch");
                return false;
            } 
            break;

        case whiledo:
            {
                struct whiledo* e = i->instr.whiledo;
                if(equal_types(get_expr_type(e->cond, functions, variables, types), (struct type*)bool_t))
                {
                    for(unsigned int i = 0; i < e->instructions.size; ++i)
                    {
                        if(!check_inst(list_nth(e->instructions,i), f, functions, variables, types))
                        {
                            printf("error in instruction switch");
                            return false;
                        }
                    }
                    return true;
                }
                printf("error in condition of switch");
                return false;
            } 
            break;

        case forloop:
            {
                struct forloop* e = i->instr.forloop;
                if(check_expr((struct expr *)e->assignment, functions, variables, types))
                {
                    if(equal_types(get_expr_type(e->upto, functions, variables, types), 
                                (struct type*)bool_t))
                    {
                        for(unsigned int i = 0; i < e->instructions.size; ++i)
                        {
                            if(!check_inst(list_nth(e->instructions, i), f, functions, variables, types))
                            {
                                printf("error in for loop");
                                return false;
                            }
                        }
                        return true;
                    }
                    printf("error in up to");
                    return false;
                }
                printf("error in assignement");
                return false;
            }
            break;

        case returnstmt:
            return equal_types(get_expr_type(i->instr.returnstmt->expr, functions, variables, types), 
                    f->ret);
            break;
    }
    return false;

}

bool check_expr(struct expr *e, fun_table_t* functions, var_table_t* variables, type_table_t* types)
{
    switch(e->exprtype)
    {
        case identtype:
            warn("check_expr ident\n");
            return(find_var(variables, e->val.ident) != NULL);
            break;

        case funcalltype:
            {
                warn("check_expr funcall\n");
                char* ident  = e->val.funcall.fun_ident;
                return get_function(functions, ident) != NULL;
            }
            break;

        case binopexprtype:
            printf("check_expr binop\n");
            return equal_types(get_expr_type(e->val.binopexpr.e1, functions, variables, types),
                    get_expr_type(e->val.binopexpr.e2, functions, variables, types));
            break;

        case unopexprtype:
            printf("check_expr unop\n");
            return get_expr_type(e->val.unopexpr.e, functions, variables, types);
            break;

        case arrayexprtype:
            printf("check_expr array\n");
            {


            }
            break;

        case structelttype:
            printf("check_expr struct\n");
            break;

        case dereftype:
            printf("check_expr deref\n");
            return(check_expr(e->val.deref.e, functions, variables, types ));
            break;

        default:
            break;
    }
    return true;

}

struct type* get_expr_type(struct expr *e, fun_table_t* functions, var_table_t* variables, 
        type_table_t* types)
{

    switch(e->exprtype)
    {
        case valtype:
            printf("val type\n");
            switch(e->val.val->valtype)
            {
                case nulltype:
                    {
                        printf("null type\n");
                        struct type* t = malloc(sizeof(struct type));
                        t->type_kind = primary_t;
                        primary_type p = nul_t;
                        t->type_val.primary = p;
                        return t;

                        break;
                    }
                case chartype:
                    {
                        printf("real type\n");
                        struct type* t = malloc(sizeof(struct type));
                        t->type_kind = primary_t;
                        primary_type p = char_t;
                        t->type_val.primary = p;
                        return t;

                        break;
                    }
                case stringtype:
                    {
                        printf("string type\n");
                        struct type* t = malloc(sizeof(struct type));
                        t->type_kind = primary_t;
                        primary_type p = str_t;
                        t->type_val.primary = p;
                        return t;

                        break;
                    }
                case booltype:
                    {
                        printf("bool type\n");
                        struct type* t = malloc(sizeof(struct type));
                        t->type_kind = primary_t;
                        primary_type p = bool_t;
                        t->type_val.primary = p;
                        return t;

                        break;
                    }
                case inttype:
                    {
                        printf("int type\n");
                        struct type* t = malloc(sizeof(struct type));
                        t->type_kind = primary_t;
                        primary_type p = int_t;
                        t->type_val.primary = p;
                        return t;

                        break;
                    }
                case realtype:
                    {
                        printf("reel type\n");
                        struct type* t = malloc(sizeof(struct type));
                        t->type_kind = primary_t;
                        primary_type p = real_t;
                        t->type_val.primary = p;
                        return t;

                        break;
                    }
            }
        case identtype:
            return find_var(variables, e->val.ident)->type;
            break;

        case funcalltype :
            {
                struct function* f = malloc(sizeof(struct function));
                if((f = get_function(functions, e->val.funcall.fun_ident)) != NULL 
                        && check_args(f, e->val.funcall, functions, variables, types)) 
                    return f->ret;
            }
            break;

        case binopexprtype:
            /*if (  equal_types(get_expr_type(e->val.binopexpr.e1, functions, variables, types),
              (struct type*)int_t)
              || equal_types(get_expr_type(e->val.binopexpr.e1, functions, variables, types),
              (struct type*)real_t)
              || equal_types(get_expr_type(e->val.binopexpr.e1, functions, variables, types), 
              (struct type*)bool_t)) 
              {*/
            if (equal_types(get_expr_type(e->val.binopexpr.e2, functions, variables, types), 
                        get_expr_type(e->val.binopexpr.e1, functions, variables, types)))
            {
                if(e->val.binopexpr.op == EQ)
                {
                    struct type* t = malloc(sizeof(struct type));
                    t->type_kind = primary_t;
                    primary_type p = bool_t;
                    t->type_val.primary = p;
                    return t;
                }
                return get_expr_type(e->val.binopexpr.e1, functions, variables, types);
            }
            else
            {
                printf("expression left was of type : ");
                printf("%s ", expr_type(e->val.binopexpr.e1));
                printf("and expression right was of type : ");
                printf("%s\n", expr_type(e->val.binopexpr.e2));
            }
            //}
            break;

        case unopexprtype:
            if (  equal_types(get_expr_type(e->val.binopexpr.e1, functions, variables, types), 
                        (struct type*)int_t)
                    || equal_types(get_expr_type(e->val.binopexpr.e1, functions, variables, types), 
                        (struct type*)real_t)
                    || equal_types(get_expr_type(e->val.binopexpr.e1, functions, variables, types), 
                        (struct type*)bool_t)) 
                return get_expr_type(e->val.unopexpr.e, functions, variables, types);

            break;

        case arrayexprtype:
            printf("array type\n");
            return get_expr_type(e->val.arrayexpr.e1, functions, variables, types);
            //something strange is happening here
            break;

        case structelttype:
            break;

        case dereftype:
            break;
    }
    return (struct type*)str_t;
}

char* expr_type (struct expr* e)
{
    switch(e->exprtype)
    {
        case valtype:
            switch(e->val.val->valtype)
            {
                case nulltype:
                    return "null";
                    break;

                case chartype:
                    return "char";
                    break;

                case stringtype:
                    return "string";

                case booltype:
                    return "bool";

                case inttype:
                    return "int";

                case realtype:
                    return "real";
            }
            break;

        case identtype:
            return "ident";
            break;

        case funcalltype :
            break;

        case binopexprtype:
            break;

        case unopexprtype:
            break;

        case arrayexprtype:
            break;

        case structelttype:
            break;

        case dereftype:
            break;

        default:
            break;
    }
    return "not yet";
}
