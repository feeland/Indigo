/****************************************************************************
 * Copyright (C) 2009-2015 EPAM Systems
 *
 * This file is part of Indigo toolkit.
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 3 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 ***************************************************************************/

#include "indigo_internal.h"
#include "molecule/molfile_saver.h"

static void setStrValue(const char* source, char* dest, int len)
{
    if(strlen(source) > len)
        throw IndigoError("invalid string value len: expected len: %d, actual len: %d", len, strlen(source));
    strcpy(dest, source);
}

static void indigoSetMolfileSavingMode (const char *mode)
{
   Indigo &self = indigoGetInstance();
   if (strcasecmp(mode, "2000") == 0)
      self.molfile_saving_mode = MolfileSaver::MODE_2000;
   else if (strcasecmp(mode, "3000") == 0)
      self.molfile_saving_mode = MolfileSaver::MODE_3000;
   else if (strcasecmp(mode, "auto") == 0)
      self.molfile_saving_mode = MolfileSaver::MODE_AUTO;
   else
      throw IndigoError("unknown value: %s", mode);
}

static void indigoGetMolfileSavingMode (char *mode, int len)
{
    Indigo &self = indigoGetInstance();
    switch(self.molfile_saving_mode)
    {
        case MolfileSaver::MODE_2000: 
            setStrValue("2000", mode, len); 
        break;
        case MolfileSaver::MODE_3000: 
            setStrValue("3000", mode, len); 
        break;
        case MolfileSaver::MODE_AUTO: 
            setStrValue("auto", mode, len); 
        break;
    }
}

static void indigoSetFilenameEncoding (const char *encoding)
{
   Indigo &self = indigoGetInstance();
   if (strcasecmp(encoding, "ASCII") == 0)
      self.filename_encoding = ENCODING_ASCII;
   else if (strcasecmp(encoding, "UTF-8") == 0)
      self.filename_encoding = ENCODING_UTF8;
   else
      throw IndigoError("unknown value: %s", encoding);
}

static void indigoGetFilenameEncoding (char *encoding, int len)
{
   Indigo &self = indigoGetInstance();
   if(self.filename_encoding == ENCODING_ASCII)
       setStrValue("ASCII", encoding, len);
   else
       setStrValue("UTF-8", encoding, len);
}

static void indigoSetLayoutOrientation(const char *orientation)
{
    Indigo &self = indigoGetInstance();
    if (strcasecmp(orientation, "unspecified") == 0)
        self.layout_orientation = 0;
    else if (strcasecmp(orientation, "horizontal") == 0)
        self.layout_orientation = 1;
    else if (strcasecmp(orientation, "vertical") == 0)
        self.layout_orientation = 2;
    else 
        throw IndigoError("unknown value: %s", orientation);
}

static void indigoGetLayoutOrientation(char *orientation, int len)
{
    Indigo &self = indigoGetInstance();
    
    switch(self.layout_orientation)
    {
        case 0: setStrValue("unspecified", orientation, len); break;
        case 1: setStrValue("horizontal", orientation, len); break;
        case 2: setStrValue("vertical", orientation, len); break;
    }
}

static void indigoSetEmbeddingUniqueness(const char *mode)
{
   Indigo &self = indigoGetInstance();
   if (strcasecmp(mode, "atoms") == 0)
   {
      self.embedding_edges_uniqueness = false;
      self.find_unique_embeddings = true;
   }
   else if (strcasecmp(mode, "bonds") == 0)
   {
      self.embedding_edges_uniqueness = true;
      self.find_unique_embeddings = true;
   }
   else if (strcasecmp(mode, "none") == 0)
   {
      self.find_unique_embeddings = false;
   }
   else
      throw IndigoError("unknown value: %s", mode);
}

static void indigoGetEmbeddingUniqueness(char *mode, int len)
{
   Indigo &self = indigoGetInstance();
   if(self.find_unique_embeddings == false)
       setStrValue("none", mode, len);
   else if(self.embedding_edges_uniqueness == false)
       setStrValue("atoms", mode, len);
   else 
       setStrValue("bonds", mode, len);
}

static void indigoSetLayoutHorIntervalFactor(float value)
{
   Indigo &self = indigoGetInstance();
   self.layout_horintervalfactor = value;
}

static void indigoGetLayoutHorIntervalFactor(float& value)
{
   Indigo &self = indigoGetInstance();
   value = self.layout_horintervalfactor;
}

static void indigoSetAromaticityModel (const char *model)
{
   Indigo &self = indigoGetInstance();
   if (strcasecmp(model, "basic") == 0)
      self.arom_options.method = AromaticityOptions::BASIC;
   else if (strcasecmp(model, "generic") == 0)
      self.arom_options.method = AromaticityOptions::GENERIC;
   else
      throw IndigoError("unknown value: %s. Allowed values are \"basic\", \"generic\"", model);
}

static void indigoGetAromaticityModel (char *model, int len)
{
   Indigo &self = indigoGetInstance();
   if(self.arom_options.method == AromaticityOptions::BASIC)
       setStrValue("basic", model, len);
   else
       setStrValue("generic", model, len);
}

static void indigoSetPkaModel (const char *model)
{
   Indigo &self = indigoGetInstance();
   if (strcasecmp(model, "simple") == 0)
      self.ionize_options.model = IonizeOptions::PKA_MODEL_SIMPLE;
   else if (strcasecmp(model, "advanced") == 0)
      self.ionize_options.model = IonizeOptions::PKA_MODEL_ADVANCED;
   else
      throw IndigoError("unknown value: %s. Allowed values are \"simple\", \"advanced\"", model);
}

static void indigoGetPkaModel (char *model, int len)
{
   Indigo &self = indigoGetInstance();
   if(self.ionize_options.model == IonizeOptions::PKA_MODEL_SIMPLE)
       setStrValue("simple", model, len);
   else 
       setStrValue("advanced", model, len);
}

static void indigoResetBasicOptions ()
{
   Indigo &self = indigoGetInstance();
   self.standardize_options.reset();
   self.ionize_options = IonizeOptions();
   self.init();
}

_IndigoBasicOptionsHandlersSetter::_IndigoBasicOptionsHandlersSetter ()
{
   OptionManager &mgr = indigoGetOptionManager();
   OsLocker locker(mgr.lock);

   #define indigo indigoGetInstance() 
   
   mgr.setOptionHandlerBool("ignore-stereochemistry-errors", SET_BOOL_OPTION(indigo.stereochemistry_options.ignore_errors));
   mgr.setOptionHandlerBool("ignore-noncritical-query-features", SET_BOOL_OPTION(indigo.ignore_noncritical_query_features));
   mgr.setOptionHandlerBool("treat-x-as-pseudoatom", SET_BOOL_OPTION(indigo.treat_x_as_pseudoatom));
   mgr.setOptionHandlerBool("skip-3d-chirality", SET_BOOL_OPTION(indigo.skip_3d_chirality));
   mgr.setOptionHandlerBool("deconvolution-aromatization", SET_BOOL_OPTION(indigo.deconvolution_aromatization));
   mgr.setOptionHandlerBool("deco-save-ap-bond-orders", SET_BOOL_OPTION(indigo.deco_save_ap_bond_orders));
   mgr.setOptionHandlerBool("deco-ignore-errors", SET_BOOL_OPTION(indigo.deco_ignore_errors));
   mgr.setOptionHandlerString("molfile-saving-mode", indigoSetMolfileSavingMode, indigoGetMolfileSavingMode);
   mgr.setOptionHandlerBool("molfile-saving-no-chiral", SET_BOOL_OPTION(indigo.molfile_saving_no_chiral));
   mgr.setOptionHandlerBool("molfile-saving-skip-date", SET_BOOL_OPTION(indigo.molfile_saving_skip_date));
   mgr.setOptionHandlerBool("molfile-saving-add-stereo-desc", SET_BOOL_OPTION(indigo.molfile_saving_add_stereo_desc));
   mgr.setOptionHandlerBool("molfile-saving-add-implicit-h", SET_BOOL_OPTION(indigo.molfile_saving_add_implicit_h));
   mgr.setOptionHandlerBool("smiles-saving-write-name", SET_BOOL_OPTION(indigo.smiles_saving_write_name));
   mgr.setOptionHandlerString("filename-encoding", indigoSetFilenameEncoding, indigoGetFilenameEncoding);
   mgr.setOptionHandlerInt("fp-ord-qwords", SET_INT_OPTION(indigo.fp_params.ord_qwords));
   mgr.setOptionHandlerInt("fp-sim-qwords", SET_INT_OPTION(indigo.fp_params.sim_qwords));
   mgr.setOptionHandlerInt("fp-any-qwords", SET_INT_OPTION(indigo.fp_params.any_qwords));
   mgr.setOptionHandlerInt("fp-tau-qwords", SET_INT_OPTION(indigo.fp_params.tau_qwords));
   mgr.setOptionHandlerBool("fp-ext-enabled", SET_BOOL_OPTION(indigo.fp_params.ext));
   mgr.setOptionHandlerBool("smart-layout", SET_BOOL_OPTION(indigo.smart_layout));
   mgr.setOptionHandlerString("layout-orientation", indigoSetLayoutOrientation, indigoGetLayoutOrientation);

   mgr.setOptionHandlerString("embedding-uniqueness", indigoSetEmbeddingUniqueness, indigoGetEmbeddingUniqueness);
   mgr.setOptionHandlerInt("max-embeddings", SET_INT_OPTION(indigo.max_embeddings));

   mgr.setOptionHandlerInt("layout-max-iterations", SET_INT_OPTION(indigo.layout_max_iterations));

   mgr.setOptionHandlerFloat("layout-horintervalfactor", indigoSetLayoutHorIntervalFactor, indigoGetLayoutHorIntervalFactor);

   mgr.setOptionHandlerInt("aam-timeout", SET_INT_OPTION(indigo.aam_cancellation_timeout));
   mgr.setOptionHandlerInt("timeout", SET_INT_OPTION(indigo.cancellation_timeout));

   mgr.setOptionHandlerBool("serialize-preserve-ordering", SET_BOOL_OPTION(indigo.preserve_ordering_in_serialize));

   mgr.setOptionHandlerString("aromaticity-model", indigoSetAromaticityModel, indigoGetAromaticityModel);
   mgr.setOptionHandlerBool("dearomatize-verification", SET_BOOL_OPTION(indigo.arom_options.dearomatize_check));
   mgr.setOptionHandlerBool("unique-dearomatization", SET_BOOL_OPTION(indigo.unique_dearomatization));
   mgr.setOptionHandlerBool("stereochemistry-bidirectional-mode", SET_BOOL_OPTION(indigo.stereochemistry_options.bidirectional_mode));
   mgr.setOptionHandlerBool("stereochemistry-detect-haworth-projection", SET_BOOL_OPTION(indigo.stereochemistry_options.detect_haworth_projection));

   mgr.setOptionHandlerBool("standardize-stereo", SET_BOOL_OPTION(indigo.standardize_options.standardize_stereo));
   mgr.setOptionHandlerBool("standardize-charges", SET_BOOL_OPTION(indigo.standardize_options.standardize_charges));
   mgr.setOptionHandlerBool("standardize-center-molecule", SET_BOOL_OPTION(indigo.standardize_options.center_molecule));
   mgr.setOptionHandlerBool("standardize-remove-single-atoms", SET_BOOL_OPTION(indigo.standardize_options.remove_single_atom_fragments));
   mgr.setOptionHandlerBool("standardize-keep-smallest", SET_BOOL_OPTION(indigo.standardize_options.keep_smallest_fragment));
   mgr.setOptionHandlerBool("standardize-keep-largest", SET_BOOL_OPTION(indigo.standardize_options.keep_largest_fragment));
   mgr.setOptionHandlerBool("standardize-remove-largest", SET_BOOL_OPTION(indigo.standardize_options.remove_largest_fragment));
   mgr.setOptionHandlerBool("standardize-make-non-h-to-c-atoms", SET_BOOL_OPTION(indigo.standardize_options.make_non_h_atoms_c_atoms));
   mgr.setOptionHandlerBool("standardize-make-non-h-to-a-atoms", SET_BOOL_OPTION(indigo.standardize_options.make_non_h_atoms_a_atoms));
   mgr.setOptionHandlerBool("standardize-make-non-h-c-to-q-atoms", SET_BOOL_OPTION(indigo.standardize_options.make_non_c_h_atoms_q_atoms));
   mgr.setOptionHandlerBool("standardize-make-all-bonds-single", SET_BOOL_OPTION(indigo.standardize_options.make_all_bonds_single));
   mgr.setOptionHandlerBool("standardize-clear-coordinates", SET_BOOL_OPTION(indigo.standardize_options.clear_coordinates));
   mgr.setOptionHandlerBool("standardize-straighten-triple-bonds", SET_BOOL_OPTION(indigo.standardize_options.straighten_triple_bonds));
   mgr.setOptionHandlerBool("standardize-straighten-allens", SET_BOOL_OPTION(indigo.standardize_options.straighten_allenes));
   mgr.setOptionHandlerBool("standardize-clear-molecule", SET_BOOL_OPTION(indigo.standardize_options.clear_molecule));
   mgr.setOptionHandlerBool("standardize-clear-stereo", SET_BOOL_OPTION(indigo.standardize_options.clear_stereo));
   mgr.setOptionHandlerBool("standardize-clear-enhanced-stereo", SET_BOOL_OPTION(indigo.standardize_options.clear_enhanced_stereo));
   mgr.setOptionHandlerBool("standardize-clear-unknown-stereo", SET_BOOL_OPTION(indigo.standardize_options.clear_unknown_stereo));
   mgr.setOptionHandlerBool("standardize-clear-unknown-atom-stereo", SET_BOOL_OPTION(indigo.standardize_options.clear_unknown_atom_stereo));
   mgr.setOptionHandlerBool("standardize-clear-unknown-bond-stereo", SET_BOOL_OPTION(indigo.standardize_options.clear_unknown_cis_trans_bond_stereo));
   mgr.setOptionHandlerBool("standardize-clear-cis-trans", SET_BOOL_OPTION(indigo.standardize_options.clear_cis_trans_bond_stereo));
   mgr.setOptionHandlerBool("standardize-stereo-from-coordinates", SET_BOOL_OPTION(indigo.standardize_options.set_stereo_from_coordinates));
   mgr.setOptionHandlerBool("standardize-reposition-stereo-bonds", SET_BOOL_OPTION(indigo.standardize_options.reposition_stereo_bonds));
   mgr.setOptionHandlerBool("standardize-reposition-axial-stereo-bonds", SET_BOOL_OPTION(indigo.standardize_options.reposition_axial_stereo_bonds));
   mgr.setOptionHandlerBool("standardize-fix-direction-wedge-bonds", SET_BOOL_OPTION(indigo.standardize_options.fix_direction_of_wedge_bonds));
   mgr.setOptionHandlerBool("standardize-clear-charges", SET_BOOL_OPTION(indigo.standardize_options.clear_charges));
   mgr.setOptionHandlerBool("standardize-highlight-colors", SET_BOOL_OPTION(indigo.standardize_options.clear_highlight_colors));
   mgr.setOptionHandlerBool("standardize-neutralize-zwitterions", SET_BOOL_OPTION(indigo.standardize_options.neutralize_bonded_zwitterions));
   mgr.setOptionHandlerBool("standardize-clear-unusual-valences", SET_BOOL_OPTION(indigo.standardize_options.clear_unusual_valence));
   mgr.setOptionHandlerBool("standardize-clear-isotopes", SET_BOOL_OPTION(indigo.standardize_options.clear_isotopes));
   mgr.setOptionHandlerBool("standardize-clear-dative-bonds", SET_BOOL_OPTION(indigo.standardize_options.clear_dative_bonds));
   mgr.setOptionHandlerBool("standardize-clear-hydrogen-bonds", SET_BOOL_OPTION(indigo.standardize_options.clear_hydrogen_bonds));
   mgr.setOptionHandlerBool("standardize-localize-markush-r-atoms-on-rings", SET_BOOL_OPTION(indigo.standardize_options.localize_markush_r_atoms_on_rings));
   mgr.setOptionHandlerBool("standardize-create-dative-bonds", SET_BOOL_OPTION(indigo.standardize_options.create_coordination_bonds));
   mgr.setOptionHandlerBool("standardize-create-hydrogen-bonds", SET_BOOL_OPTION(indigo.standardize_options.create_hydrogen_bonds));
   mgr.setOptionHandlerBool("standardize-remove-extra-stereo-bonds", SET_BOOL_OPTION(indigo.standardize_options.remove_extra_stereo_bonds));

   mgr.setOptionHandlerString("pKa-model", indigoSetPkaModel, indigoGetPkaModel);
   mgr.setOptionHandlerInt("pKa-model-level", SET_INT_OPTION(indigo.ionize_options.level));
   mgr.setOptionHandlerInt("pKa-model-min-level", SET_INT_OPTION(indigo.ionize_options.min_level));

   mgr.setOptionHandlerVoid("reset-basic-options", indigoResetBasicOptions);

   mgr.setOptionHandlerBool("mass-skip-error-on-pseudoatoms", SET_BOOL_OPTION(indigo.mass_options.skip_error_on_pseudoatoms));
   mgr.setOptionHandlerBool("gross-formula-add-rsites", SET_BOOL_OPTION(indigo.gross_formula_options.add_rsites));
}

_IndigoBasicOptionsHandlersSetter::~_IndigoBasicOptionsHandlersSetter ()
{
}
