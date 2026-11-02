"""Native counter names for independent induction-value components."""
from scalar_lifetime_source import analyze as values,project as split
from getter_lifetime_source import GETTER_CALLS,name_available


def analyze(text):return [g for g in values(text,calls=GETTER_CALLS,loop_roles=True) if g['roles']==['loop_index']]


def project(before,selected):return split(before,selected,calls=GETTER_CALLS,allow_field_names=True,loop_roles=True)
