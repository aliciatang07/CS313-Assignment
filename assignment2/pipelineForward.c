#include "pipeline.h"
#include <stdbool.h>

/* Pipeline control unit. This implementation uses data forwarding to
 handle hazards, in addition to stalls when required. */
int pipelineControl(const int_reg_fd_t *f, const int_reg_de_t *d, const int_reg_em_t *e,
                    const int_reg_mw_t *m, const int_reg_ret_t *w,
                    int_reg_fetch_t *F, int_reg_fd_t *D, int_reg_de_t *E, int_reg_em_t *M,
                    int_reg_mw_t *W) {
    
    /* TO BE COMPLETED BY THE STUDENT */
    
    
    //check hazard first and forward all together
    //check all possible data hazard and record in variable
    bool DE_hazard_A = false;
    bool DE_hazard_B = false;
    bool DM_hazard_AE = false;
    bool DM_hazard_AM = false;
    bool DM_hazard_BE = false;
    bool DM_hazard_BM = false;
    bool DW_hazard_AE = false;
    bool DW_hazard_AM = false;
    bool DW_hazard_BE = false;
    bool DW_hazard_BM = false;
    bool DE_hazard_RSP = false;
    bool DE_hazard_RSP_A = false;
    bool DM_hazard_RSP = false;
    bool DM_hazard_RSP_A = false;
    bool DW_hazard_RSP = false;
    bool DW_hazard_RSP_A = false;
    
    //between decode and write back
    if(D->rA!=R_NONE&&(D->rA == W->dstE||D->rA == W->dstM)){
        if(m->iCode == 2 && m->iFun!=0 && m->bch ==0){
            //don't move, no data hazard
            F->status = ST_REGULAR;
            *D = *f;
            *E = *d;
            *M = *e;
            *W = *m;
            D->status = ST_REGULAR;
            E->status = ST_REGULAR;
            M->status = ST_REGULAR;
            return w->status != ST_EXCEPTION && w->iCode != I_HALT;
        }
        if(D->rA == W->dstE){
            DW_hazard_AE = true;
        }else{
            DW_hazard_AM = true;
        }
    }
    
    if(D->iCode == I_OPQ && D->rB!=R_NONE&&(D->rB == W->dstE||D->rB == W->dstM)) {
        
        if(D->rB == W->dstE){
            DW_hazard_BE = true;
        }else{
            DW_hazard_BM = true;
        }
    }
    if((D->iCode == I_POPQ||D->iCode==I_PUSHQ||D->iCode == I_RET||D->iCode ==I_CALL) && W->dstE ==R_RSP){
        DW_hazard_RSP = true;
        if(D->iCode == I_POPQ||D->iCode == I_RET){
            DW_hazard_RSP_A = true;
        }
    }
    
    //between decode and memory
    //forward valE/ValM to valA next cycle
    if(D->rA!=R_NONE&&(D->rA == M->dstE||D->rA == M->dstM)){
        if(e->iCode == 2 && e->iFun!=0 && e->bch ==0){
            F->status = ST_REGULAR;
            *D = *f;
            *E = *d;
            *M = *e;
            *W = *m;
            D->status = ST_REGULAR;
            E->status = ST_REGULAR;
            M->status = ST_REGULAR;
            return w->status != ST_EXCEPTION && w->iCode != I_HALT;
        }
        if(D->rA == M->dstE){
            DM_hazard_AE = true;
        } else{
            DM_hazard_AM = true;
        }
        
    }
    
    if(D->iCode == I_OPQ && D->rB!=R_NONE&&(D->rB == M->dstE||D->rB == M->dstM)) {
        if(D->rB == M->dstE){
            DM_hazard_BE = true;
        } else{
            DM_hazard_BM = true;
        }
    }
    if((D->iCode == I_POPQ||D->iCode==I_PUSHQ||D->iCode == I_RET||D->iCode ==I_CALL) && m->dstE ==R_RSP){
        DM_hazard_RSP = true;
        if(D->iCode == I_POPQ||D->iCode == I_RET){
            DM_hazard_RSP_A = true;
        }
    }
    
    // between decode and execute
    if(D->rA!=R_NONE&&(D->rA == e->dstE||D->rA == e->dstM)){
        if(E->iCode == I_MRMOVQ || (E->iCode == I_POPQ && E->srcA!=R_RSP)){
            *D = *f;
            *E = *d;
            *M = *e;
            *W = *m;
            D->status = ST_REGULAR;
            E->status = ST_BUBBLE;
            E->iCode = 1;
            E->iFun = 0;
            F->PC = f->nextPC;
            F->status = ST_REGULAR;
            return w->status != ST_EXCEPTION && w->iCode != I_HALT;
        }
        DE_hazard_A = true;
    }
    
    if(D->iCode == I_OPQ&&D->rB!=R_NONE&&(D->rB == e->dstE||D->rB == e->dstM)) {
        DE_hazard_B = true;
    }
    if((D->iCode == I_POPQ||D->iCode==I_PUSHQ||D->iCode == I_RET||D->iCode ==I_CALL) && e->dstE ==R_RSP){
        DE_hazard_RSP = true;
        if(D->iCode == I_POPQ||D->iCode == I_RET){
            DE_hazard_RSP_A = true;
        }
    }
    
    if(f->rA!=R_NONE&&(f->rA == d->dstE||f->rA == d->dstM)&&(d->iCode == I_MRMOVQ ||(d->iCode == I_POPQ && d->srcA!=R_RSP))){
        //F->status = ST_STALLED;
        if(e->iCode == 2 && e->iFun!=0 && e->bch ==0){
            //do not move, no data hazard;
            F->PC = f->nextPC;
            *D = *f;
            *E = *d;
            *M = *e;
            *W = *m;
            return w->status != ST_EXCEPTION && w->iCode != I_HALT;
        }
        F->PC = f->nextPC;
        F->status = ST_STALLED;
        *D = *f;
        *E = *d;
        *M = *e;
        *W = *m;
        D->status = ST_STALLED;
        return w->status != ST_EXCEPTION && w->iCode != I_HALT;
    }
    
    
    bool datahazard = DE_hazard_A || DE_hazard_B || DM_hazard_AE||DM_hazard_AM||
    DM_hazard_BE|| DM_hazard_BM || DW_hazard_AE|| DW_hazard_AM ||DW_hazard_BE||
    DW_hazard_BM ||DE_hazard_RSP ||DM_hazard_RSP|| DW_hazard_RSP;
    
    if(datahazard){
        bool retcase = false;
        if(D->iCode == I_RET){
            retcase = true;
        }
        F->PC = f->nextPC;
        F->status = ST_REGULAR;
        *E = *d;
        *D = *f;
        if(DW_hazard_AM){
            E->valA = W->valM;
        }
        if(DW_hazard_BM){
            E->valB = W->valM;
        }
        if(DW_hazard_AE||DW_hazard_RSP_A){
            E->valA = W->valE;
        }
        if(DW_hazard_BE||DW_hazard_RSP){
            E->valB = W->valE;
        }
        
        *W = *m;
        if(DM_hazard_AM){
            E->valA = m->valM;
        }
        if(DM_hazard_BM){
            E->valB = m->valM;
        }
        if(DM_hazard_AE||DM_hazard_RSP_A){
            E->valA = M->valE;
        }
        if(DM_hazard_BE||DM_hazard_RSP){
            E->valB = M->valE;
        }
        *M = *e;
        if(DE_hazard_A||DE_hazard_RSP_A){
            E->valA = e->valE;
        }
        if(DE_hazard_B||DE_hazard_RSP){
            E->valB = e->valE;
        }
        
        if(retcase){
            return w->status != ST_EXCEPTION && w->iCode != I_HALT;
        }
    }
    bool cndjmp = false;
    bool ret = false;
    //jxx
    if(e->iCode == 7 && e->iFun !=0 && E->status!= ST_BUBBLE){
        cndjmp = true;
        if (e->bch == 1) {
            F->PC = E->valC;
        } else {
            F->PC = E->valA;
        }
        F->status = ST_REGULAR;
        *D = *f;
        *E = *d;
        *M = *e;
        *W = *m;
        D->status = ST_BUBBLE;
        D->iCode = 1;
        D->iFun = 0;
        return w->status != ST_EXCEPTION && w->iCode != I_HALT;
    }
    if(d->iCode == 7 && d->iFun !=0 && D->status!= ST_BUBBLE){
        cndjmp = true;
        F->status = ST_STALLED;
        F->PC = f->nextPC;
        *D = *f;
        *E = *d;
        *M = *e;
        *W = *m;
        D->status = ST_BUBBLE;
        D->iCode = 1;
        D->iFun = 0;
        return w->status != ST_EXCEPTION && w->iCode != I_HALT;
    }
    if(f->iCode == 7 && f->iFun !=0 && F->status!= ST_STALLED){
        cndjmp = true;
        F->status = ST_STALLED;
        F->PC = f->nextPC;
        *D = *f;
        *E = *d;
        *M = *e;
        *W = *m;
        return w->status != ST_EXCEPTION && w->iCode != I_HALT;
    }
    
    //RET
    if(m->iCode == 9 && m->iFun == 0 && M->status != ST_BUBBLE){
        ret = true;
        *D = *f;
        *E = *d;
        *M = *e;
        *W = *m;
        F-> PC = m ->valM;
        F-> status = ST_REGULAR;
        D-> status = ST_BUBBLE;
        E->status = ST_BUBBLE;
        M->status = ST_BUBBLE;
        D->iCode = 1;
        D->iFun = 0;
        E->iCode = 1;
        E->iFun = 0;
        M->iCode = 1;
        M->iFun = 0;
        
        return w->status != ST_EXCEPTION && w->iCode != I_HALT;
    }
    if(e->iCode ==9 ){
        ret = true;
        //F->PC = f->nextPC;
        *D = *f;
        *E = *d;
        *M = *e;
        *W = *m;
        //F->status = ST_STALLED;
        D->status = ST_BUBBLE;
        E->status = ST_BUBBLE;
        D->iCode = 1;
        D->iFun = 0;
        E->iCode = 1;
        E->iFun = 0;
        return w->status != ST_EXCEPTION && w->iCode != I_HALT;
    }
    if(d->iCode == 9){
        ret = true;
        *D = *f;
        *E = *d;
        *M = *e;
        *W = *m;
        // F->status = ST_STALLED;
        F->PC = f->nextPC;
        D->status = ST_BUBBLE;
        D->iCode = 1;
        D->iFun = 0;
        return w->status != ST_EXCEPTION && w->iCode != I_HALT;
    }
    if(f->iCode == 9){
        if(d->iCode == I_RET || d->iCode == I_POPQ || d->iCode == I_PUSHQ ||d->iCode == I_CALL
           ||e->iCode == I_RET || e->iCode == I_POPQ || e->iCode == I_PUSHQ ||e->iCode == I_CALL
           ||m->iCode == I_RET || m->iCode == I_POPQ || m->iCode == I_PUSHQ ||m->iCode == I_CALL
           ||d->srcA == R_RSP|| d->srcB == R_RSP || e->dstE == R_RSP || m->dstE ==R_RSP
           ){
            if(!datahazard){
                F->status = ST_REGULAR;
                F->PC = f->nextPC;
                *D = *f;
                *E = *d;
                *M = *e;
                *W = *m;
            }
            return w->status != ST_EXCEPTION && w->iCode != I_HALT;
        }
        ret = true;
        F->status = ST_STALLED;
        F->PC = f->nextPC;
        *D = *f;
        *E = *d;
        *M = *e;
        *W = *m;
        return w->status != ST_EXCEPTION && w->iCode != I_HALT;
    }
    if(!datahazard && !ret && !cndjmp){
        F->status = ST_REGULAR;
        F->PC = f->nextPC;
        *D = *f;
        *E = *d;
        *M = *e;
        *W = *m;
        return w->status != ST_EXCEPTION && w->iCode != I_HALT;
    }
    return w->status != ST_EXCEPTION && w->iCode != I_HALT;
}

