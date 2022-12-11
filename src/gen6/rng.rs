use super::reader::Gen6Reader;
use crate::rng::{RngWrapper, TinyMT, MT};

#[derive(Default)]
pub struct Gen6Rng {
    mt: RngWrapper<MT>,
    tinymt: RngWrapper<TinyMT>,
}

impl Gen6Rng {
    pub fn update(&mut self, reader: &Gen6Reader) {
        let init_seed = reader.initial_seed();
        let mt_state = reader.mt_state();
        let tinymt_state = reader.tinymt_state();

        let reseeded = self.mt.reinit_if_needed(init_seed);
        if reseeded {
            self.tinymt.reinit(tinymt_state);
            self.tinymt.set_current_state(tinymt_state);
        }

        self.mt.update_advances(mt_state);
        self.tinymt.update_advances(tinymt_state);
    }

    pub fn mt(&self) -> &RngWrapper<MT> {
        &self.mt
    }

    pub fn tinymt(&self) -> &RngWrapper<TinyMT> {
        &self.tinymt
    }
}
