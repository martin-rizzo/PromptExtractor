/*
| File    : print_params.c
| Purpose : Prints the `PeParams` structure to stdout with terminal color formatting.
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 17, 2026
| Repo    : https://github.com/martin-rizzo/PromptExtractor
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                              PromptExtractor
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#include <stdio.h>
#include <promptex/promptex.h>
#include "termcolors.h"

static void
_section(const char* name, const TermColors* c) {
    printf("\n%s[%s]%s\n", c->highlight, name, c->reset);
}

static void
_prt_string(const char* name, const PeString* str, const TermColors* c) {
    if (str->has_value) {
        printf("%s%s: %s\"%s\"%s\n", c->label, name, c->string, str->value, c->reset);
    }
}

static void
_prt_long(const char* name, const PeLong* long_, const TermColors* c) {
    if (long_->has_value) {
        printf("%s%s: %s%ld%s\n", c->label, name, c->data, long_->value, c->reset);
    }
}

static void
_prt_float(const char* name, const PeFloat* float_, const TermColors* c) {
    if (float_->has_value) {
        printf("%s%s: %s%f%s\n", c->label, name, c->data, float_->value, c->reset);
    }
}


/*===========================================================================
|  ////////////////////////// PUBLIC FUNCTIONS ///////////////////////////  | 
===========================================================================*/

void print_params(const PeParams* p, const TermColors* c) {
    _section("Main", c);
    _prt_string("Prompt"   , &p->prompt         , c);
    _prt_string("Negative" , &p->negative_prompt, c);
    _prt_string("Wildcard" , &p->wildcard_prompt, c);
    _prt_string("Sampler"  , &p->sampler        , c);
    _prt_long  ("Steps"    , &p->steps          , c);
    _prt_float ("CFG"      , &p->cfg            , c);
    _prt_long  ("Seed"     , &p->seed           , c);
    _prt_long  ("Width"    , &p->width          , c);
    _prt_long  ("Height"   , &p->height         , c);
    _prt_float ("Denoising", &p->denoising      , c);
    
    if (p->model.name.has_value || p->model.hash.has_value) {
        _section("Model", c);
        _prt_string("Model Name", &p->model.name, c);
        _prt_string("Model Hash", &p->model.hash, c);
    }
   
    if (p->hires.upscaler.has_value     || p->hires.steps.has_value      ||
        p->hires.denoising.has_value    || p->hires.upscale.has_value    ||
        p->hires.width.has_value        || p->hires.height.has_value     ||
        p->hires.calc_upscale.has_value || p->hires.calc_width.has_value ||
        p->hires.calc_height.has_value  ){
        _section("Hires Fix", c);
        _prt_string("Hires Upscaler"    , &p->hires.upscaler    , c);
        _prt_long  ("Hires Steps"       , &p->hires.steps       , c);
        _prt_float ("Hires Denoising"   , &p->hires.denoising   , c);
        _prt_float ("Hires Upscale"     , &p->hires.upscale     , c);
        _prt_long  ("Hires Width"       , &p->hires.width       , c);
        _prt_long  ("Hires Height"      , &p->hires.height      , c);
        _prt_float ("Hires Calc Upscale", &p->hires.calc_upscale, c);
        _prt_long  ("Hires Calc Width"  , &p->hires.calc_width  , c);
        _prt_long  ("Hires Calc Height" , &p->hires.calc_height , c);
    }
    if (p->inpaint.denoising.has_value || p->inpaint.mask_blur.has_value) {
        _section("Inpaint", c);
        _prt_float("Inpaint Denoising", &p->inpaint.denoising, c);
        _prt_float("Inpaint Mask Blur", &p->inpaint.mask_blur, c);
    }
    if (p->settings.eta.has_value       || p->settings.ensd.has_value ||
        p->settings.clip_skip.has_value ){
        _section("Settings", c);
        _prt_float("Eta"      , &p->settings.eta      , c);
        _prt_long ("Ensd"     , &p->settings.ensd     , c);
        _prt_long ("Clip Skip", &p->settings.clip_skip, c);
    }
}
