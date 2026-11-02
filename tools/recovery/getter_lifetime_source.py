"""Native-vocabulary lifetimes for additional recovered integer getter APIs."""
from role_local_source import CALLS
from scalar_lifetime_source import analyze as analyze_values,project as project_values,name_available as available

# get_card_name returns a CSV card id, as its recovered implementation shows;
# the string lookup is the separate pointer-returning get_card_name_by_id API.
GETTER_CALLS={**CALLS,
    'get_card_name':['csvid','csv_id','card_id'],
    'get_original_id':['original_csvid','original_id','csvid','csv_id'],
    'get_original_internal_card_id':['original_iid','iid','internal_card_id'],
    'get_internal_card_id_from_csv_id':['iid','internal_card_id','internal_id'],
    'get_cmc_by_id':['cmc','mana_value'],
    'get_color':['color','colors','color_test','col'],
    'get_color_by_internal_id_no_hack':['color','colors','color_test','col'],
    'get_base_power':['base_power','base_pow','power','pow'],
    'get_base_power_iid':['base_power','base_pow','power','pow'],
    'get_base_toughness':['base_toughness','base_tgh','toughness','tgh'],
    'get_base_toughness_iid':['base_toughness','base_tgh','toughness','tgh'],
    'get_owner':['owner','card_owner'],
}


def analyze(text):return analyze_values(text,calls=GETTER_CALLS)


def name_available(text,name):return available(text,name,allow_fields=True)


def project(before,selected):return project_values(before,selected,calls=GETTER_CALLS,allow_field_names=True)
